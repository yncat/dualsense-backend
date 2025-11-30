// Device Manager Implementation

#include "device_manager.h"
#include "../hid/windows_hid.h"
#include "../protocol/output_composer.h"
#include <cstring>
#include <cstdio>

namespace dualsense {

DeviceManager& DeviceManager::Instance() {
    static DeviceManager instance;
    return instance;
}

DSResult DeviceManager::Initialize() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (device_.is_connected) {
        return DS_ERROR_ALREADY_CONNECTED;
    }

    // Detect devices
    std::vector<DeviceInfo> devices;
    if (!hid::DetectDevices(devices) || devices.empty()) {
        return DS_ERROR_NOT_FOUND;
    }

    // Connect to first DualSense device found
    for (const auto& device_info : devices) {
        if (device_info.device_type == DS_DEVICE_DUALSENSE ||
            device_info.device_type == DS_DEVICE_DUALSENSE_EDGE) {

            device_.path = device_info.path;
            device_.device_type = device_info.device_type;
            device_.connection_type = device_info.connection_type;

            device_.handle = hid::OpenDevice(device_.path);
            if (device_.handle == INVALID_HANDLE_VALUE) {
                printf("DeviceManager: Failed to open device\n");
                return DS_ERROR_IO_FAILED;
            }

            device_.is_connected = true;

            printf("DeviceManager: Connected to %s via %s\n",
                   (device_.device_type == DS_DEVICE_DUALSENSE_EDGE) ? "DualSense Edge" : "DualSense",
                   (device_.connection_type == DS_CONNECTION_BLUETOOTH) ? "Bluetooth" : "USB");

            return DS_OK;
        }
    }

    return DS_ERROR_NOT_FOUND;
}

void DeviceManager::Shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (device_.is_connected) {
        // Reset all effects before disconnecting
        device_.output.lightbar = {};
        device_.output.rumbles = {};
        device_.output.left_trigger.mode = 0x0;
        device_.output.right_trigger.mode = 0x0;
        WriteOutput();

        hid::CloseDevice(device_.handle);
        device_.handle = INVALID_HANDLE_VALUE;
        device_.is_connected = false;

        printf("DeviceManager: Disconnected\n");
    }
}

bool DeviceManager::IsConnected() const {
    return device_.is_connected;
}

DSConnectionType DeviceManager::GetConnectionType() const {
    return static_cast<DSConnectionType>(device_.connection_type);
}

DSDeviceType DeviceManager::GetDeviceType() const {
    return static_cast<DSDeviceType>(device_.device_type);
}

DSResult DeviceManager::UpdateInput() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!device_.is_connected) {
        return DS_ERROR_NOT_CONNECTED;
    }

    const size_t input_size = (device_.connection_type == DS_CONNECTION_BLUETOOTH) ? 78 : 64;
    unsigned long bytes_read = 0;

    if (!hid::ReadInputReport(device_.handle, device_.buffer_input, input_size, &bytes_read)) {
        // Check if device disconnected
        if (!hid::PingDevice(device_.handle)) {
            device_.is_connected = false;
            return DS_ERROR_DISCONNECTED;
        }
        return DS_ERROR_IO_FAILED;
    }

    // Input parsing will be implemented here
    // For now, return success
    return DS_OK;
}

DSResult DeviceManager::GetInputState(DSInputState* out_state) {
    if (!out_state) {
        return DS_ERROR_INVALID_PARAM;
    }

    if (!device_.is_connected) {
        return DS_ERROR_NOT_CONNECTED;
    }

    // TODO: Parse input buffer into DSInputState
    // For now, zero it out
    memset(out_state, 0, sizeof(DSInputState));
    return DS_OK;
}

DSResult DeviceManager::SetLightbar(uint8_t r, uint8_t g, uint8_t b) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!device_.is_connected) {
        return DS_ERROR_NOT_CONNECTED;
    }

    device_.output.lightbar.r = r;
    device_.output.lightbar.g = g;
    device_.output.lightbar.b = b;

    return WriteOutput();
}

DSResult DeviceManager::SetPlayerLed(DSLedPlayer led, DSLedBrightness brightness) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!device_.is_connected) {
        return DS_ERROR_NOT_CONNECTED;
    }

    device_.output.player_led.led = static_cast<uint8_t>(led);
    device_.output.player_led.brightness = static_cast<uint8_t>(brightness);

    return WriteOutput();
}

DSResult DeviceManager::SetMicLed(DSLedMic mode) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!device_.is_connected) {
        return DS_ERROR_NOT_CONNECTED;
    }

    device_.output.mic_light.mode = static_cast<uint8_t>(mode);

    return WriteOutput();
}

DSResult DeviceManager::SetRumble(uint8_t left, uint8_t right) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!device_.is_connected) {
        return DS_ERROR_NOT_CONNECTED;
    }

    device_.output.rumbles.left = left;
    device_.output.rumbles.right = right;

    return WriteOutput();
}

DSResult DeviceManager::StopRumble() {
    return SetRumble(0, 0);
}

void DeviceManager::ApplyTriggerEffect(HapticTriggers& trigger, uint8_t mode, const uint8_t params[10]) {
    trigger.mode = mode;
    if (params) {
        memcpy(trigger.strengths.compose, params, 10);
    }
}

DSResult DeviceManager::TriggerOff(bool left, bool right) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!device_.is_connected) {
        return DS_ERROR_NOT_CONNECTED;
    }

    if (left) device_.output.left_trigger.mode = 0x0;
    if (right) device_.output.right_trigger.mode = 0x0;

    return WriteOutput();
}

DSResult DeviceManager::TriggerContinuousResistance(bool left, bool right, uint8_t start_pos, uint8_t force) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!device_.is_connected) {
        return DS_ERROR_NOT_CONNECTED;
    }

    uint8_t params[10] = { start_pos, force, 0 };

    if (left) ApplyTriggerEffect(device_.output.left_trigger, 0x01, params);
    if (right) ApplyTriggerEffect(device_.output.right_trigger, 0x01, params);

    return WriteOutput();
}

DSResult DeviceManager::TriggerBow(bool left, bool right, uint8_t start, uint8_t end, uint8_t str_start, uint8_t str_end) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!device_.is_connected) {
        return DS_ERROR_NOT_CONNECTED;
    }

    uint8_t params[10] = { start, end, str_start, str_end, 0 };

    if (left) ApplyTriggerEffect(device_.output.left_trigger, 0x02, params);
    if (right) ApplyTriggerEffect(device_.output.right_trigger, 0x02, params);

    return WriteOutput();
}

DSResult DeviceManager::TriggerGalloping(bool left, bool right, uint8_t start, uint8_t end, uint8_t first, uint8_t second, uint8_t freq) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!device_.is_connected) {
        return DS_ERROR_NOT_CONNECTED;
    }

    uint8_t params[10] = { start, end, first, second, freq, 0 };

    if (left) ApplyTriggerEffect(device_.output.left_trigger, 0x23, params);
    if (right) ApplyTriggerEffect(device_.output.right_trigger, 0x23, params);

    return WriteOutput();
}

DSResult DeviceManager::TriggerResistance(bool left, bool right, uint8_t str_start, uint8_t str_mid, uint8_t str_end) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!device_.is_connected) {
        return DS_ERROR_NOT_CONNECTED;
    }

    uint8_t params[10] = { 0, 0, str_start, str_mid, str_end, 0 };

    if (left) ApplyTriggerEffect(device_.output.left_trigger, 0x21, params);
    if (right) ApplyTriggerEffect(device_.output.right_trigger, 0x21, params);

    return WriteOutput();
}

DSResult DeviceManager::TriggerWeapon(bool left, bool right, uint8_t start, uint8_t end, uint8_t strength) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!device_.is_connected) {
        return DS_ERROR_NOT_CONNECTED;
    }

    uint8_t params[10] = { start, end, strength, 0 };

    if (left) ApplyTriggerEffect(device_.output.left_trigger, 0x25, params);
    if (right) ApplyTriggerEffect(device_.output.right_trigger, 0x25, params);

    return WriteOutput();
}

DSResult DeviceManager::TriggerAutomaticGun(bool left, bool right, uint8_t start, uint8_t strength, uint8_t freq) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!device_.is_connected) {
        return DS_ERROR_NOT_CONNECTED;
    }

    uint8_t params[10] = { start, strength, freq, 0 };

    if (left) ApplyTriggerEffect(device_.output.left_trigger, 0x26, params);
    if (right) ApplyTriggerEffect(device_.output.right_trigger, 0x26, params);

    return WriteOutput();
}

DSResult DeviceManager::TriggerMachine(bool left, bool right, uint8_t start, uint8_t amplitude, uint8_t freq) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!device_.is_connected) {
        return DS_ERROR_NOT_CONNECTED;
    }

    uint8_t params[10] = { start, amplitude, freq, 0 };

    if (left) ApplyTriggerEffect(device_.output.left_trigger, 0x27, params);
    if (right) ApplyTriggerEffect(device_.output.right_trigger, 0x27, params);

    return WriteOutput();
}

DSResult DeviceManager::TriggerCustom(bool left, bool right, const uint8_t params[10]) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!device_.is_connected) {
        return DS_ERROR_NOT_CONNECTED;
    }

    if (!params) {
        return DS_ERROR_INVALID_PARAM;
    }

    if (left) ApplyTriggerEffect(device_.output.left_trigger, 0xFF, params);
    if (right) ApplyTriggerEffect(device_.output.right_trigger, 0xFF, params);

    return WriteOutput();
}

DSResult DeviceManager::SendAudioHaptic(const uint8_t* data, uint32_t size) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!device_.is_connected) {
        return DS_ERROR_NOT_CONNECTED;
    }

    if (device_.connection_type != DS_CONNECTION_BLUETOOTH) {
        printf("DeviceManager: Audio haptics only supported on Bluetooth\n");
        return DS_ERROR_INVALID_PARAM;
    }

    if (!data || size > 142) {
        return DS_ERROR_INVALID_PARAM;
    }

    memcpy(device_.buffer_audio, data, size);
    protocol::SendAudioHapticAdvanced(&device_);

    return DS_OK;
}

DSResult DeviceManager::ResetAll() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!device_.is_connected) {
        return DS_ERROR_NOT_CONNECTED;
    }

    // Reset all outputs
    device_.output.lightbar = {};
    device_.output.rumbles = {};
    device_.output.left_trigger.mode = 0x0;
    device_.output.right_trigger.mode = 0x0;
    device_.output.mic_light.mode = 0x0;

    return WriteOutput();
}

DSResult DeviceManager::FlushOutput() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!device_.is_connected) {
        return DS_ERROR_NOT_CONNECTED;
    }

    return WriteOutput();
}

DSResult DeviceManager::WriteOutput() {
    // Call appropriate output composer based on device type
    if (device_.device_type == DS_DEVICE_DUALSHOCK4) {
        protocol::OutputDualShock(&device_);
    }
    else {
        protocol::OutputDualSense(&device_);
    }

    return DS_OK;
}

} // namespace dualsense
