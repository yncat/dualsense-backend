// DualSense C API Implementation
// Bridges C API to C++ DeviceManager

#include "../../include/dualsense.h"
#include "device_manager.h"

using namespace dualsense;

extern "C" {

// ========================================
// Device Management
// ========================================

DUALSENSE_API DSResult ds_init(void) {
    return DeviceManager::Instance().Initialize();
}

DUALSENSE_API void ds_shutdown(void) {
    DeviceManager::Instance().Shutdown();
}

DUALSENSE_API bool ds_is_connected(void) {
    return DeviceManager::Instance().IsConnected();
}

DUALSENSE_API DSConnectionType ds_get_connection_type(void) {
    return DeviceManager::Instance().GetConnectionType();
}

DUALSENSE_API DSDeviceType ds_get_device_type(void) {
    return DeviceManager::Instance().GetDeviceType();
}

// ========================================
// Input Reading
// ========================================

DUALSENSE_API DSResult ds_update_input(void) {
    return DeviceManager::Instance().UpdateInput();
}

DUALSENSE_API DSResult ds_get_input_state(DSInputState* out_state) {
    return DeviceManager::Instance().GetInputState(out_state);
}

// ========================================
// LED Control
// ========================================

DUALSENSE_API DSResult ds_set_lightbar(uint8_t r, uint8_t g, uint8_t b) {
    return DeviceManager::Instance().SetLightbar(r, g, b);
}

DUALSENSE_API DSResult ds_set_player_led(DSLedPlayer led, DSLedBrightness brightness) {
    return DeviceManager::Instance().SetPlayerLed(led, brightness);
}

DUALSENSE_API DSResult ds_set_mic_led(DSLedMic mode) {
    return DeviceManager::Instance().SetMicLed(mode);
}

// ========================================
// Vibration Control
// ========================================

DUALSENSE_API DSResult ds_set_rumble(uint8_t left, uint8_t right) {
    return DeviceManager::Instance().SetRumble(left, right);
}

DUALSENSE_API DSResult ds_stop_rumble(void) {
    return DeviceManager::Instance().StopRumble();
}

// ========================================
// Adaptive Trigger Effects
// ========================================

DUALSENSE_API DSResult ds_trigger_off(bool left, bool right) {
    return DeviceManager::Instance().TriggerOff(left, right);
}

DUALSENSE_API DSResult ds_trigger_continuous_resistance(bool left, bool right, uint8_t start_position, uint8_t force) {
    return DeviceManager::Instance().TriggerContinuousResistance(left, right, start_position, force);
}

DUALSENSE_API DSResult ds_trigger_bow(bool left, bool right, uint8_t start_position, uint8_t end_position, uint8_t strength_start, uint8_t strength_end) {
    return DeviceManager::Instance().TriggerBow(left, right, start_position, end_position, strength_start, strength_end);
}

DUALSENSE_API DSResult ds_trigger_galloping(bool left, bool right, uint8_t start_position, uint8_t end_position, uint8_t first_foot, uint8_t second_foot, uint8_t frequency) {
    return DeviceManager::Instance().TriggerGalloping(left, right, start_position, end_position, first_foot, second_foot, frequency);
}

DUALSENSE_API DSResult ds_trigger_resistance(bool left, bool right, uint8_t strength_start, uint8_t strength_mid, uint8_t strength_end) {
    return DeviceManager::Instance().TriggerResistance(left, right, strength_start, strength_mid, strength_end);
}

DUALSENSE_API DSResult ds_trigger_weapon(bool left, bool right, uint8_t start_position, uint8_t end_position, uint8_t strength) {
    return DeviceManager::Instance().TriggerWeapon(left, right, start_position, end_position, strength);
}

DUALSENSE_API DSResult ds_trigger_automatic_gun(bool left, bool right, uint8_t start_position, uint8_t strength, uint8_t frequency) {
    return DeviceManager::Instance().TriggerAutomaticGun(left, right, start_position, strength, frequency);
}

DUALSENSE_API DSResult ds_trigger_machine(bool left, bool right, uint8_t start_position, uint8_t amplitude, uint8_t frequency) {
    return DeviceManager::Instance().TriggerMachine(left, right, start_position, amplitude, frequency);
}

DUALSENSE_API DSResult ds_trigger_custom(bool left, bool right, const uint8_t params[10]) {
    return DeviceManager::Instance().TriggerCustom(left, right, params);
}

// ========================================
// Audio Haptics
// ========================================

DUALSENSE_API DSResult ds_send_audio_haptic(const uint8_t* data, uint32_t size) {
    return DeviceManager::Instance().SendAudioHaptic(data, size);
}

// ========================================
// Utility Functions
// ========================================

DUALSENSE_API DSResult ds_reset_all(void) {
    return DeviceManager::Instance().ResetAll();
}

DUALSENSE_API DSResult ds_flush_output(void) {
    return DeviceManager::Instance().FlushOutput();
}

} // extern "C"
