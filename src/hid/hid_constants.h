// DualSense HID Constants
// Migrated from Unreal Engine plugin to standalone C++ library

#pragma once

#include <stdint.h>

// ========================================
// Sony VID/PID
// ========================================
#define SONY_VENDOR_ID 0x054C
#define DUALSENSE_PRODUCT_ID 0x0CE6
#define DUALSENSE_EDGE_PRODUCT_ID 0x0DF2
#define DUALSHOCK4_PRODUCT_ID 0x05C4
#define DUALSHOCK4_V2_PRODUCT_ID 0x09CC

// ========================================
// Button Masks
// ========================================
#define BTN_FN1 0x10
#define BTN_FN2 0x20
#define BTN_PADDLE_LEFT 0x40
#define BTN_PADDLE_RIGHT 0x80

#define BTN_DPAD_UP 0x8
#define BTN_DPAD_DOWN 0x02
#define BTN_DPAD_LEFT 0x01
#define BTN_DPAD_RIGHT 0x04

#define BTN_CROSS 0x20
#define BTN_SQUARE 0x10
#define BTN_CIRCLE 0x40
#define BTN_TRIANGLE 0x80

#define BTN_START 0x20
#define BTN_SELECT 0x10

#define BTN_LEFT_STICK 0x40
#define BTN_RIGHT_STICK 0x80

#define BTN_LEFT_SHOULDER 0x01
#define BTN_RIGHT_SHOULDER 0x02

#define BTN_LEFT_TRIGGER 0x04
#define BTN_RIGHT_TRIGGER 0x08

#define BTN_PAD_BUTTON 0x02
#define BTN_MIC_BUTTON 0x04
#define BTN_PLAYSTATION_LOGO 0x01

// ========================================
// Player LED Masks
// ========================================
#define PLAYER_LED_LEFT 0x01
#define PLAYER_LED_RIGHT 0x10
#define PLAYER_LED_MIDDLE 0x04
#define PLAYER_LED_MIDDLE_LEFT 0x02
#define PLAYER_LED_MIDDLE_RIGHT 0x08

// ========================================
// Touchpad Data Offsets
// ========================================
#define TOUCHPAD1_OFFSET 0x20
#define TOUCHPAD2_OFFSET 0x24

// Touchpad bit masks and shifts
#define TOUCH_ID_MASK 0x0000007F      // Bits 0-6
#define TOUCH_DOWN_BIT (1 << 7)       // Bit 7 (0=down, 1=up)
#define TOUCH_X_MASK 0x000FFF00       // Bits 8-19
#define TOUCH_Y_MASK 0xFFF00000       // Bits 20-31
#define TOUCH_X_SHIFT 8
#define TOUCH_Y_SHIFT 20

// Note: DSConnectionType, DSDeviceType, DSLedMic, DSLedPlayer, and DSLedBrightness
// are defined in dualsense.h (public API header)

// Internal device type constant (not exposed in public API)
#define DS_DEVICE_DUALSHOCK4 2

// ========================================
// Trigger Effect Modes
// ========================================
enum DSTriggerMode {
    DS_TRIGGER_OFF = 0x00,
    DS_TRIGGER_CONTINUOUS_RESISTANCE = 0x01,
    DS_TRIGGER_BOW = 0x02,
    DS_TRIGGER_RESISTANCE = 0x21,
    DS_TRIGGER_BOW_ALT = 0x22,
    DS_TRIGGER_GALLOPING = 0x23,
    DS_TRIGGER_WEAPON = 0x25,
    DS_TRIGGER_AUTOMATIC_GUN = 0x26,
    DS_TRIGGER_MACHINE = 0x27,
    DS_TRIGGER_CUSTOM = 0xFF
};

// ========================================
// Feature Report Settings
// ========================================
enum DSVibrationMode {
    DS_VIBRATION_DEFAULT = 0xFF,
    DS_VIBRATION_HAPTIC_SOFT = 0xFC,
    DS_VIBRATION_OFF = 0x00
};

enum DSFeatureMode {
    DS_FEATURE_DEFAULT = 0xF7
};

// ========================================
// Audio Configuration
// ========================================
enum DSAudioMode {
    DS_AUDIO_DEFAULT = 0x05
};

#define DS_AUDIO_VOLUME_DEFAULT 0x7C
