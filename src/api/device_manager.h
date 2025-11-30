// Device Manager - Singleton for managing global device state
// This manages a single DualSense device

#pragma once

#include "../core/device_context.h"
#include "../hid/hid_constants.h"
#include "../../include/dualsense.h"
#include <mutex>

namespace dualsense {

class DeviceManager {
public:
    // Get singleton instance
    static DeviceManager& Instance();

    // Initialize and connect to device
    DSResult Initialize();

    // Shutdown and disconnect
    void Shutdown();

    // Check connection status
    bool IsConnected() const;

    // Get device information
    DSConnectionType GetConnectionType() const;
    DSDeviceType GetDeviceType() const;

    // Input reading
    DSResult UpdateInput();
    DSResult GetInputState(DSInputState* out_state);

    // LED control
    DSResult SetLightbar(uint8_t r, uint8_t g, uint8_t b);
    DSResult SetPlayerLed(DSLedPlayer led, DSLedBrightness brightness);
    DSResult SetMicLed(DSLedMic mode);

    // Vibration control
    DSResult SetRumble(uint8_t left, uint8_t right);
    DSResult StopRumble();

    // Trigger effects
    DSResult TriggerOff(bool left, bool right);
    DSResult TriggerContinuousResistance(bool left, bool right, uint8_t start_pos, uint8_t force);
    DSResult TriggerBow(bool left, bool right, uint8_t start, uint8_t end, uint8_t str_start, uint8_t str_end);
    DSResult TriggerGalloping(bool left, bool right, uint8_t start, uint8_t end, uint8_t first, uint8_t second, uint8_t freq);
    DSResult TriggerResistance(bool left, bool right, uint8_t str_start, uint8_t str_mid, uint8_t str_end);
    DSResult TriggerWeapon(bool left, bool right, uint8_t start, uint8_t end, uint8_t strength);
    DSResult TriggerAutomaticGun(bool left, bool right, uint8_t start, uint8_t strength, uint8_t freq);
    DSResult TriggerMachine(bool left, bool right, uint8_t start, uint8_t amplitude, uint8_t freq);
    DSResult TriggerCustom(bool left, bool right, const uint8_t params[10]);

    // Audio haptics
    DSResult SendAudioHaptic(const uint8_t* data, uint32_t size);

    // Utility
    DSResult ResetAll();
    DSResult FlushOutput();

private:
    DeviceManager() = default;
    ~DeviceManager() = default;
    DeviceManager(const DeviceManager&) = delete;
    DeviceManager& operator=(const DeviceManager&) = delete;

    // Internal helpers
    void ApplyTriggerEffect(HapticTriggers& trigger, uint8_t mode, const uint8_t params[10]);
    DSResult WriteOutput();

    // Device state
    DeviceContext device_;
    std::mutex mutex_;
};

} // namespace dualsense
