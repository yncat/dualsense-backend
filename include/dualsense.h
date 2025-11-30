// DualSense C API
// Windows-only DLL for controlling Sony DualSense controllers

#pragma once

#ifndef DUALSENSE_H
#define DUALSENSE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DUALSENSE_EXPORTS
#define DUALSENSE_API __declspec(dllexport)
#else
#define DUALSENSE_API __declspec(dllimport)
#endif

#include <stdint.h>
#include <stdbool.h>

// ========================================
// Error Codes
// ========================================
typedef enum {
    DS_OK = 0,
    DS_ERROR_NOT_FOUND = -1,
    DS_ERROR_ALREADY_CONNECTED = -2,
    DS_ERROR_NOT_CONNECTED = -3,
    DS_ERROR_INVALID_PARAM = -4,
    DS_ERROR_IO_FAILED = -5,
    DS_ERROR_DISCONNECTED = -6
} DSResult;

// ========================================
// Device Enumerations
// ========================================
typedef enum {
    DS_CONNECTION_USB = 0,
    DS_CONNECTION_BLUETOOTH = 1,
    DS_CONNECTION_UNKNOWN = 2
} DSConnectionType;

typedef enum {
    DS_DEVICE_DUALSENSE = 0,
    DS_DEVICE_DUALSENSE_EDGE = 1,
    DS_DEVICE_NOT_FOUND = 255
} DSDeviceType;

// ========================================
// LED Enumerations
// ========================================
typedef enum {
    DS_LED_MIC_OFF = 0x0,
    DS_LED_MIC_ON = 0x1,
    DS_LED_MIC_PULSE = 0x2
} DSLedMic;

typedef enum {
    DS_LED_PLAYER_OFF = 0,
    DS_LED_PLAYER_1 = 0x04,
    DS_LED_PLAYER_2 = 0x0A,
    DS_LED_PLAYER_3 = 0x15,
    DS_LED_PLAYER_ALL = 0x1F
} DSLedPlayer;

typedef enum {
    DS_LED_BRIGHTNESS_HIGH = 0x00,
    DS_LED_BRIGHTNESS_MEDIUM = 0x02,
    DS_LED_BRIGHTNESS_LOW = 0x03
} DSLedBrightness;

// ========================================
// Touchpad State Structure
// ========================================
// Touchpad coordinates use raw 12-bit values (0-4095)
// To normalize to 0.0-1.0 range: normalized = raw / 4095.0f
typedef struct {
    uint16_t x;              // X coordinate (0-4095)
    uint16_t y;              // Y coordinate (0-4095)
    uint8_t id;              // Touch ID (0-9)
    bool is_active;          // True if touch is currently down
} DSTouchPoint;

// ========================================
// Input State Structure
// ========================================
typedef struct {
    // Face buttons
    bool button_cross;
    bool button_circle;
    bool button_square;
    bool button_triangle;

    // Shoulder buttons
    bool button_l1;
    bool button_r1;
    bool button_l2_digital;
    bool button_r2_digital;

    // Stick buttons
    bool button_l3;
    bool button_r3;

    // D-pad
    bool button_dpad_up;
    bool button_dpad_down;
    bool button_dpad_left;
    bool button_dpad_right;

    // System buttons
    bool button_create;
    bool button_options;
    bool button_ps;
    bool button_mute;
    bool button_touchpad;

    // Analog sticks (0-255, center=128)
    uint8_t stick_lx;
    uint8_t stick_ly;
    uint8_t stick_rx;
    uint8_t stick_ry;

    // Analog triggers (0-255)
    uint8_t trigger_l2;
    uint8_t trigger_r2;

    // Battery level (0-100, or negative if charging)
    int8_t battery_level;
    bool battery_charging;

    // Touchpad (2-point multi-touch)
    DSTouchPoint touch1;
    DSTouchPoint touch2;
} DSInputState;

// ========================================
// Device Management
// ========================================

// Initialize and connect to DualSense device
DUALSENSE_API DSResult ds_init(void);

// Shutdown and disconnect
DUALSENSE_API void ds_shutdown(void);

// Check connection status
DUALSENSE_API bool ds_is_connected(void);

// Get device information
DUALSENSE_API DSConnectionType ds_get_connection_type(void);
DUALSENSE_API DSDeviceType ds_get_device_type(void);

// ========================================
// Input Reading
// ========================================

// Update input state (call this once per frame)
DUALSENSE_API DSResult ds_update_input(void);

// Get current input state
DUALSENSE_API DSResult ds_get_input_state(DSInputState* out_state);

// ========================================
// LED Control
// ========================================

// Set lightbar color (RGB 0-255)
DUALSENSE_API DSResult ds_set_lightbar(uint8_t r, uint8_t g, uint8_t b);

// Set player LED indicator
DUALSENSE_API DSResult ds_set_player_led(DSLedPlayer led, DSLedBrightness brightness);

// Set microphone LED
DUALSENSE_API DSResult ds_set_mic_led(DSLedMic mode);

// ========================================
// Vibration/Rumble Control
// ========================================

// Set rumble intensity (0-255 for left and right motors)
DUALSENSE_API DSResult ds_set_rumble(uint8_t left, uint8_t right);

// Stop all rumble
DUALSENSE_API DSResult ds_stop_rumble(void);

// ========================================
// Adaptive Trigger Effects
// ========================================

// Turn off trigger effects
DUALSENSE_API DSResult ds_trigger_off(bool left, bool right);

// Continuous resistance effect
DUALSENSE_API DSResult ds_trigger_continuous_resistance(
    bool left, bool right,
    uint8_t start_position,  // 0-9
    uint8_t force            // 0-8
);

// Bow effect (increasing resistance)
DUALSENSE_API DSResult ds_trigger_bow(
    bool left, bool right,
    uint8_t start_position,  // 0-8
    uint8_t end_position,    // 0-8
    uint8_t strength_start,  // 0-8
    uint8_t strength_end     // 0-8
);

// Galloping effect (rhythmic resistance)
DUALSENSE_API DSResult ds_trigger_galloping(
    bool left, bool right,
    uint8_t start_position,  // 0-8
    uint8_t end_position,    // 0-8
    uint8_t first_foot,      // 0-7
    uint8_t second_foot,     // 0-7
    uint8_t frequency        // 0-255
);

// Section-based resistance
DUALSENSE_API DSResult ds_trigger_resistance(
    bool left, bool right,
    uint8_t strength_start,  // 0-8
    uint8_t strength_mid,    // 0-8
    uint8_t strength_end     // 0-8
);

// Weapon effect (section-based resistance)
DUALSENSE_API DSResult ds_trigger_weapon(
    bool left, bool right,
    uint8_t start_position,  // 0-8
    uint8_t end_position,    // 0-8
    uint8_t strength         // 0-8
);

// Automatic gun effect
DUALSENSE_API DSResult ds_trigger_automatic_gun(
    bool left, bool right,
    uint8_t start_position,  // 0-8
    uint8_t strength,        // 0-7
    uint8_t frequency        // 0-255
);

// Machine/vibration effect
DUALSENSE_API DSResult ds_trigger_machine(
    bool left, bool right,
    uint8_t start_position,  // 0-8
    uint8_t amplitude,       // 0-7
    uint8_t frequency        // 0-255
);

// Custom trigger effect (raw 10-byte parameters)
DUALSENSE_API DSResult ds_trigger_custom(
    bool left, bool right,
    const uint8_t params[10]
);

// ========================================
// Audio Haptics (Bluetooth only)
// ========================================

// Send audio haptic data
DUALSENSE_API DSResult ds_send_audio_haptic(const uint8_t* data, uint32_t size);

// ========================================
// Utility Functions
// ========================================

// Reset all effects (LED, rumble, triggers)
DUALSENSE_API DSResult ds_reset_all(void);

// Flush output immediately
DUALSENSE_API DSResult ds_flush_output(void);

#ifdef __cplusplus
}
#endif

#endif // DUALSENSE_H
