// DualSense Protocol Output Composer
// Migrated from PlayStationOutputComposer.cpp (283 lines)
// Ref: https://github.com/rafaelvaloto/WindowsDualsenseUnreal

#include "output_composer.h"
#include "crc32.h"
#include "../hid/windows_hid.h"
#include "../hid/hid_constants.h"
#include "../../include/dualsense.h"
#include <cstring>
#include <cstdio>

namespace dualsense {
namespace protocol {

void OutputDualShock(DeviceContext* device_context) {
    const OutputContext* hid_out = &device_context->output;

    size_t padding = (device_context->connection_type == DS_CONNECTION_BLUETOOTH) ? 2 : 1;
    device_context->buffer_output[0] = (device_context->connection_type == DS_CONNECTION_BLUETOOTH) ? 0x11 : 0x05;

    if (device_context->connection_type == DS_CONNECTION_BLUETOOTH) {
        device_context->buffer_output[1] = 0xc0;
    }

    unsigned char* output = &device_context->buffer_output[padding];

    if (device_context->connection_type == DS_CONNECTION_BLUETOOTH) {
        output[0] = 0x20;
        output[1] = 0x07;
    }
    else {
        output[0] = 0xff;
    }

    output[3 + (padding - 1)] = hid_out->rumbles.left;
    output[4 + (padding - 1)] = hid_out->rumbles.right;
    output[5 + (padding - 1)] = hid_out->lightbar.r;
    output[6 + (padding - 1)] = hid_out->lightbar.g;
    output[7 + (padding - 1)] = hid_out->lightbar.b;
    output[8 + (padding - 1)] = hid_out->flash_lightbar.bright_time;
    output[9 + (padding - 1)] = hid_out->flash_lightbar.toggle_time;

    if (device_context->connection_type == DS_CONNECTION_BLUETOOTH) {
        const uint32_t crc_checksum = ComputeCRC32(device_context->buffer_output, 74);
        device_context->buffer_output[0x4A] = static_cast<unsigned char>((crc_checksum & 0x000000FF) >> 0);
        device_context->buffer_output[0x4B] = static_cast<unsigned char>((crc_checksum & 0x0000FF00) >> 8);
        device_context->buffer_output[0x4C] = static_cast<unsigned char>((crc_checksum & 0x00FF0000) >> 16);
        device_context->buffer_output[0x4D] = static_cast<unsigned char>((crc_checksum & 0xFF000000) >> 24);
    }

    hid::WriteOutputReport(device_context->handle, device_context->buffer_output,
        (device_context->connection_type == DS_CONNECTION_BLUETOOTH) ? 78 : 32);
}

void OutputDualSense(DeviceContext* device_context) {
    const size_t padding = (device_context->connection_type == DS_CONNECTION_BLUETOOTH) ? 2 : 1;
    device_context->buffer_output[0] = (device_context->connection_type == DS_CONNECTION_BLUETOOTH) ? 0x31 : 0x02;

    if (device_context->connection_type == DS_CONNECTION_BLUETOOTH) {
        device_context->buffer_output[1] = 0x02;
    }

    OutputContext* hid_out = &device_context->output;
    unsigned char* output = &device_context->buffer_output[padding];

    output[0] = hid_out->feature.vibration_mode;
    output[1] = hid_out->feature.feature_mode;
    output[2] = hid_out->rumbles.left;
    output[3] = hid_out->rumbles.right;
    output[4] = hid_out->audio.headset_volume;
    output[5] = hid_out->audio.speaker_volume;
    output[6] = hid_out->audio.mic_volume;
    output[7] = hid_out->audio.mode;
    output[9] = hid_out->audio.mic_status;
    output[8] = hid_out->mic_light.mode;
    output[36] = (hid_out->feature.trigger_softness_level << 4) | (hid_out->feature.soft_rumble_reduce & 0x0F);
    output[38] = 0x07;
    output[41] = 0x02;
    output[42] = hid_out->player_led.brightness;
    output[43] = hid_out->player_led.led;
    output[44] = hid_out->lightbar.r;
    output[45] = hid_out->lightbar.g;
    output[46] = hid_out->lightbar.b;

    if (device_context->override_trigger_bytes) {
        memcpy(&output[10], device_context->override_trigger_right, 10);
        memcpy(&output[21], device_context->override_trigger_left, 10);
    }
    else {
        SetTriggerEffects(&output[10], hid_out->right_trigger);
        SetTriggerEffects(&output[21], hid_out->left_trigger);
    }

    if (device_context->connection_type == DS_CONNECTION_BLUETOOTH) {
        const uint32_t crc_checksum = ComputeCRC32(device_context->buffer_output, 74);
        device_context->buffer_output[0x4A] = static_cast<unsigned char>((crc_checksum & 0x000000FF) >> 0);
        device_context->buffer_output[0x4B] = static_cast<unsigned char>((crc_checksum & 0x0000FF00) >> 8);
        device_context->buffer_output[0x4C] = static_cast<unsigned char>((crc_checksum & 0x00FF0000) >> 16);
        device_context->buffer_output[0x4D] = static_cast<unsigned char>((crc_checksum & 0xFF000000) >> 24);
    }

    size_t output_length = (device_context->connection_type == DS_CONNECTION_BLUETOOTH) ? 78 : 74;
    hid::WriteOutputReport(device_context->handle, device_context->buffer_output, output_length);
}

void SetTriggerEffects(unsigned char* trigger, HapticTriggers& effect) {
    trigger[0x0] = effect.mode;

    if (effect.mode == 0x01) { // Continuous Resistance
        trigger[0x1] = static_cast<unsigned char>((effect.strengths.active_zones >> 0) & 0xFF);
        trigger[0x2] = static_cast<unsigned char>((effect.strengths.strength_zones >> 0) & 0xFF);
    }

    if (effect.mode == 0x21) { // Resistance
        trigger[0x1] = 0xf0;
        trigger[0x2] = 0x03;
        trigger[0x3] = 0x00;
        trigger[0x5] = effect.strengths.compose[2];
        trigger[0x6] = effect.strengths.compose[3];
        trigger[0x7] = 0x0;
        trigger[0x8] = 0x0;
        trigger[0x9] = 0x0;
    }

    if (effect.mode == 0x22 || effect.mode == 0x02) { // Bow
        trigger[0x1] = effect.strengths.compose[0];
        trigger[0x2] = effect.strengths.compose[1];
        trigger[0x3] = effect.strengths.compose[2];
        trigger[0x4] = 0x0;
        trigger[0x5] = 0x0;
        trigger[0x6] = 0x0;
        trigger[0x7] = 0x0;
        trigger[0x8] = 0x0;
        trigger[0x9] = 0x0;
    }

    if (effect.mode == 0x23) { // Galloping
        trigger[0x1] = effect.strengths.compose[0];
        trigger[0x2] = effect.strengths.compose[1];
        trigger[0x3] = effect.strengths.compose[2];
        trigger[0x4] = effect.strengths.compose[3];
        trigger[0x5] = 0x00;
        trigger[0x6] = 0x00;
        trigger[0x7] = 0x00;
        trigger[0x8] = 0x00;
        trigger[0x9] = 0x00;
    }

    if (effect.mode == 0x25) { // Weapon
        trigger[0x1] = static_cast<unsigned char>((effect.strengths.active_zones >> 0) & 0xFF);
        trigger[0x2] = static_cast<unsigned char>((effect.strengths.active_zones >> 8) & 0xFF);
        for (int i = 0; i < 8; ++i) {
            trigger[0x3 + i] = static_cast<unsigned char>((effect.strengths.strength_zones >> (8 * i)) & 0xFF);
        }
    }

    if (effect.mode == 0x26) { // Automatic Gun
        trigger[0x1] = effect.strengths.compose[0];
        trigger[0x2] = effect.strengths.compose[1];
        trigger[0x3] = effect.strengths.compose[2];
        trigger[0x4] = effect.strengths.compose[3];
        trigger[0x5] = effect.strengths.compose[4];
        trigger[0x6] = effect.strengths.compose[5];
        trigger[0x7] = 0x0;
        trigger[0x8] = 0x0;
        trigger[0x9] = effect.strengths.compose[9];
    }

    if (effect.mode == 0x27) { // Machine Advanced
        trigger[0x1] = effect.strengths.compose[0]; // Start_Zone
        trigger[0x2] = effect.strengths.compose[1]; // Behavior_Flag
        trigger[0x3] = effect.strengths.compose[2]; // Force_Amplitude
        trigger[0x4] = effect.strengths.compose[3]; // Period
        trigger[0x5] = effect.strengths.compose[4]; // Frequency
        trigger[0x6] = 0x00;
        trigger[0x7] = 0x00;
        trigger[0x8] = 0x00;
        trigger[0x9] = 0x00;
    }

    if (effect.mode == 0xFF) { // Custom Mode effect
        trigger[0x0] = effect.strengths.compose[0];
        trigger[0x1] = effect.strengths.compose[1];
        trigger[0x2] = effect.strengths.compose[2];
        trigger[0x3] = effect.strengths.compose[3];
        trigger[0x4] = effect.strengths.compose[4];
        trigger[0x5] = effect.strengths.compose[5];
        trigger[0x6] = effect.strengths.compose[6];
        trigger[0x7] = effect.strengths.compose[7];
        trigger[0x8] = effect.strengths.compose[8];
        trigger[0x9] = effect.strengths.compose[9];
    }

    if (effect.mode == 0x0) { // Reset
        trigger[0x1] = 0x0;
        trigger[0x2] = 0x0;
        trigger[0x3] = 0x0;
        trigger[0x4] = 0x0;
        trigger[0x5] = 0x0;
        trigger[0x6] = 0x0;
        trigger[0x7] = 0x0;
        trigger[0x8] = 0x0;
        trigger[0x9] = 0x0;
    }
}

void SendAudioHapticAdvanced(DeviceContext* device_context) {
    if (!device_context) {
        return;
    }

    if (device_context->connection_type == DS_CONNECTION_BLUETOOTH) {
        constexpr size_t crc_offset = 138;
        const uint32_t crc_checksum = ComputeCRC32(device_context->buffer_audio, crc_offset);
        device_context->buffer_audio[crc_offset + 0] = static_cast<unsigned char>((crc_checksum & 0x000000FF) >> 0);
        device_context->buffer_audio[crc_offset + 1] = static_cast<unsigned char>((crc_checksum & 0x0000FF00) >> 8);
        device_context->buffer_audio[crc_offset + 2] = static_cast<unsigned char>((crc_checksum & 0x00FF0000) >> 16);
        device_context->buffer_audio[crc_offset + 3] = static_cast<unsigned char>((crc_checksum & 0xFF000000) >> 24);
        hid::WriteAudioHaptic(device_context->handle, device_context->buffer_audio, 142);
    }
}

} // namespace protocol
} // namespace dualsense
