// Windows HID Communication Layer
// Migrated from WindowsDeviceInfo.h
// Ref: https://github.com/rafaelvaloto/WindowsDualsenseUnreal

#pragma once

#include <Windows.h>
#include <string>
#include <vector>

// Forward declarations
struct DeviceInfo {
    std::wstring path;
    int device_type;        // DSDeviceType
    int connection_type;    // DSConnectionType
};

namespace dualsense {
namespace hid {

// Detect all connected DualSense devices
bool DetectDevices(std::vector<DeviceInfo>& out_devices);

// Open device handle
HANDLE OpenDevice(const std::wstring& path);

// Close device handle
void CloseDevice(HANDLE handle);

// Read input report from device
bool ReadInputReport(HANDLE handle, unsigned char* buffer, size_t size, unsigned long* bytes_read);

// Write output report to device
bool WriteOutputReport(HANDLE handle, const unsigned char* buffer, size_t size);

// Ping device to check connection status
bool PingDevice(HANDLE handle);

// Configure Bluetooth features (feature report 0x05)
bool ConfigureBluetoothFeatures(HANDLE handle);

// Write audio haptic data (Bluetooth only)
bool WriteAudioHaptic(HANDLE handle, const unsigned char* buffer, size_t size);

} // namespace hid
} // namespace dualsense
