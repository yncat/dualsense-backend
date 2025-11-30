// DualSense Protocol Output Composer
// Migrated from PlayStationOutputComposer.h/cpp
// Ref: https://github.com/rafaelvaloto/WindowsDualsenseUnreal

#pragma once

#include "../core/device_context.h"

namespace dualsense {
namespace protocol {

// Compose and write DualSense output report
void OutputDualSense(DeviceContext* device_context);

// Compose and write DualShock output report
void OutputDualShock(DeviceContext* device_context);

// Set trigger effect parameters
void SetTriggerEffects(unsigned char* trigger_bytes, HapticTriggers& effect);

// Send audio haptic data (Bluetooth only)
void SendAudioHapticAdvanced(DeviceContext* device_context);

} // namespace protocol
} // namespace dualsense
