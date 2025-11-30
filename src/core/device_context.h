// Device Context Structure
// Migrated from DeviceContext.h (212 lines) - simplified for C++ DLL

#pragma once

#include "output_context.h"
#include <Windows.h>
#include <string>

namespace dualsense {

// Device context - holds all device state
struct DeviceContext {
    // Device handles
    HANDLE handle = INVALID_HANDLE_VALUE;
    HANDLE audio_handle = INVALID_HANDLE_VALUE;

    // Device path
    std::wstring path;

    // HID buffers
    unsigned char buffer_input[78] = {};
    unsigned char buffer_ds4[547] = {};
    unsigned char buffer_audio[142] = {};
    unsigned char buffer_output[78] = {};

    // Connection status
    bool is_connected = false;

    // Output settings
    OutputContext output;

    // Connection type (DSConnectionType enum values)
    int connection_type = 2;  // DS_CONNECTION_UNKNOWN

    // Device type (DSDeviceType enum values)
    int device_type = 255;  // DS_DEVICE_NOT_FOUND

    // Runtime trigger override
    bool override_trigger_bytes = false;
    unsigned char override_trigger_right[10] = {};
    unsigned char override_trigger_left[10] = {};
};

} // namespace dualsense
