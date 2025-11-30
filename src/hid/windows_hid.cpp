// Windows HID Communication Layer
// Migrated from WindowsDeviceInfo.cpp (289 lines)
// Ref: https://github.com/rafaelvaloto/WindowsDualsenseUnreal

#include "windows_hid.h"
#include "hid_constants.h"
#include "../../include/dualsense.h"
#include <hidsdi.h>
#include <setupapi.h>
#include <cstdio>
#include <cstring>

namespace dualsense {
namespace hid {

bool DetectDevices(std::vector<DeviceInfo>& out_devices) {
    out_devices.clear();

    GUID hid_guid;
    HidD_GetHidGuid(&hid_guid);

    const HDEVINFO device_info_set = SetupDiGetClassDevs(
        &hid_guid, nullptr, nullptr,
        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    if (device_info_set == INVALID_HANDLE_VALUE) {
        printf("HIDManager: Failed to get HID device information\n");
        return false;
    }

    SP_DEVICE_INTERFACE_DATA device_interface_data = {};
    device_interface_data.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    std::vector<std::wstring> device_paths;

    for (int device_index = 0;
         SetupDiEnumDeviceInterfaces(device_info_set, nullptr, &hid_guid, device_index, &device_interface_data);
         device_index++) {

        DWORD required_size = 0;
        SetupDiGetDeviceInterfaceDetail(device_info_set, &device_interface_data, nullptr, 0, &required_size, nullptr);

        auto detail_data_buffer = static_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(malloc(required_size));
        if (!detail_data_buffer) {
            printf("HIDManager: Failed to allocate memory for device details\n");
            continue;
        }

        detail_data_buffer->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        if (SetupDiGetDeviceInterfaceDetail(device_info_set, &device_interface_data, detail_data_buffer, required_size, nullptr, nullptr)) {
            const HANDLE temp_device_handle = CreateFileW(
                detail_data_buffer->DevicePath,
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                nullptr,
                OPEN_EXISTING,
                NULL,
                nullptr);

            if (temp_device_handle != INVALID_HANDLE_VALUE) {
                HIDD_ATTRIBUTES attributes = {};
                attributes.Size = sizeof(HIDD_ATTRIBUTES);

                if (HidD_GetAttributes(temp_device_handle, &attributes)) {
                    // Check if this is a Sony device (DualSense or DualShock)
                    if (attributes.VendorID == SONY_VENDOR_ID &&
                        (attributes.ProductID == DUALSENSE_PRODUCT_ID ||
                         attributes.ProductID == DUALSENSE_EDGE_PRODUCT_ID ||
                         attributes.ProductID == DUALSHOCK4_PRODUCT_ID ||
                         attributes.ProductID == DUALSHOCK4_V2_PRODUCT_ID)) {

                        WCHAR device_product_string[260];
                        if (HidD_GetProductString(temp_device_handle, device_product_string, 260)) {
                            std::wstring path_str(detail_data_buffer->DevicePath);

                            // Check if we haven't already added this device
                            bool already_added = false;
                            for (const auto& existing_path : device_paths) {
                                if (existing_path == path_str) {
                                    already_added = true;
                                    break;
                                }
                            }

                            if (!already_added) {
                                DeviceInfo context = {};
                                context.path = path_str;
                                device_paths.push_back(path_str);

                                // Determine device type
                                switch (attributes.ProductID) {
                                    case DUALSHOCK4_PRODUCT_ID:
                                    case DUALSHOCK4_V2_PRODUCT_ID:
                                        context.device_type = DS_DEVICE_DUALSHOCK4;
                                        break;
                                    case DUALSENSE_EDGE_PRODUCT_ID:
                                        context.device_type = DS_DEVICE_DUALSENSE_EDGE;
                                        break;
                                    default:
                                        context.device_type = DS_DEVICE_DUALSENSE;
                                }

                                // Determine connection type (USB or Bluetooth)
                                context.connection_type = DS_CONNECTION_USB;
                                if (path_str.find(L"{00001124-0000-1000-8000-00805f9b34fb}") != std::wstring::npos ||
                                    path_str.find(L"bth") != std::wstring::npos ||
                                    path_str.find(L"BTHENUM") != std::wstring::npos) {
                                    context.connection_type = DS_CONNECTION_BLUETOOTH;

                                    // Configure Bluetooth features
                                    if (!ConfigureBluetoothFeatures(temp_device_handle)) {
                                        printf("HIDManager: Warning - Failed to configure Bluetooth features\n");
                                    }
                                }

                                out_devices.push_back(context);
                            }
                        }
                        else {
                            printf("HIDManager: Failed to obtain product string for device\n");
                        }
                    }
                }
                CloseHandle(temp_device_handle);
            }
        }
        free(detail_data_buffer);
    }

    SetupDiDestroyDeviceInfoList(device_info_set);
    return !out_devices.empty();
}

HANDLE OpenDevice(const std::wstring& path) {
    HANDLE device_handle = CreateFileW(
        path.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        NULL,
        nullptr);

    if (device_handle == INVALID_HANDLE_VALUE) {
        printf("HIDManager: Failed to open device handle. Error: %lu\n", GetLastError());
        return INVALID_HANDLE_VALUE;
    }

    return device_handle;
}

void CloseDevice(HANDLE handle) {
    if (handle != INVALID_HANDLE_VALUE) {
        CloseHandle(handle);
    }
}

bool ReadInputReport(HANDLE handle, unsigned char* buffer, size_t size, unsigned long* bytes_read) {
    if (handle == INVALID_HANDLE_VALUE) {
        printf("HIDManager: Invalid device handle before attempting to read\n");
        return false;
    }

    // Flush any old data
    HidD_FlushQueue(handle);

    DWORD read = 0;
    if (!ReadFile(handle, buffer, static_cast<DWORD>(size), &read, nullptr)) {
        if (bytes_read) *bytes_read = 0;
        return false;
    }

    if (bytes_read) *bytes_read = read;
    return true;
}

bool WriteOutputReport(HANDLE handle, const unsigned char* buffer, size_t size) {
    if (handle == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD bytes_written = 0;
    if (!WriteFile(handle, buffer, static_cast<DWORD>(size), &bytes_written, nullptr)) {
        printf("HIDManager: Failed to write output report. Size: %zu, Error: %lu\n",
               size, GetLastError());
        return false;
    }

    return true;
}

bool PingDevice(HANDLE handle) {
    if (handle == INVALID_HANDLE_VALUE) {
        return false;
    }

    FILE_STANDARD_INFO info{};
    if (!GetFileInformationByHandleEx(handle, FileStandardInfo, &info, sizeof(info))) {
        return false;
    }

    return true;
}

bool ConfigureBluetoothFeatures(HANDLE device_handle) {
    // Feature Report 0x05 - Enables advanced Bluetooth features
    unsigned char feature_buffer[41];
    memset(feature_buffer, 0, sizeof(feature_buffer));
    feature_buffer[0] = 0x05;

    if (!HidD_GetFeature(device_handle, feature_buffer, 41)) {
        const DWORD error = GetLastError();
        printf("HIDManager: Failed to get Feature 0x05. Error: %lu\n", error);
        return false;
    }

    return true;
}

bool WriteAudioHaptic(HANDLE handle, const unsigned char* buffer, size_t size) {
    if (!handle || handle == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD bytes_written = 0;
    constexpr size_t expected_size = 142;

    if (size != expected_size) {
        printf("HIDManager: Warning - Audio haptic buffer size is %zu, expected %zu\n",
               size, expected_size);
    }

    if (!WriteFile(handle, buffer, static_cast<DWORD>(size), &bytes_written, nullptr)) {
        const DWORD error = GetLastError();
        if (error != ERROR_IO_PENDING) {
            printf("HIDManager: Failed to send audio haptics. Error: %lu\n", error);
        }
        return false;
    }

    return true;
}

} // namespace hid
} // namespace dualsense
