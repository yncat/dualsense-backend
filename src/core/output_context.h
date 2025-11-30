// Output Context Structures
// Migrated from OutputContext.h (646 lines) - simplified for C++ DLL

#pragma once

#include <stdint.h>

namespace dualsense {

// Lightbar RGB color
struct Lightbar {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 0;
};

// Rumble motors
struct Rumbles {
    uint8_t left = 0;
    uint8_t right = 0;
};

// Player LED
struct PlayerLed {
    uint8_t led = 0;
    uint8_t brightness = 0;
};

// Microphone LED
struct MicLight {
    uint8_t mode = 0;
};

// Audio configuration
struct AudioConfig {
    uint8_t mode = 0x05;
    uint8_t headset_volume = 0x7C;
    uint8_t speaker_volume = 0x7C;
    uint8_t mic_volume = 0x7C;
    uint8_t mic_status = 0x0;
};

// Feature configuration
struct FeatureConfig {
    uint8_t feature_mode = 0xF7;
    uint8_t vibration_mode = 0xFF;
    uint8_t soft_rumble_reduce = 0x01;
    uint8_t trigger_softness_level = 0x01;
};

// Trigger effect strengths/parameters
struct TriggerStrengths {
    uint8_t period = 0;
    uint32_t active_zones = 0;
    uint32_t time_and_ratio = 0;
    uint64_t strength_zones = 0;
    uint8_t compose[10] = {0};
};

// Haptic trigger configuration
struct HapticTriggers {
    uint8_t mode = 0x0;
    uint8_t frequency = 0x0;
    uint8_t amplitude = 0x0;
    uint8_t keep_effect = 0x0;
    TriggerStrengths strengths;
};

// DualShock flash lightbar
struct DualShockFlashLightbar {
    uint8_t bright_time = 0x0;
    uint8_t toggle_time = 0x0;
};

// Complete output context
struct OutputContext {
    Lightbar lightbar;
    DualShockFlashLightbar flash_lightbar;
    MicLight mic_light;
    PlayerLed player_led;
    Rumbles rumbles;
    AudioConfig audio;
    FeatureConfig feature;
    HapticTriggers left_trigger;
    HapticTriggers right_trigger;
};

} // namespace dualsense
