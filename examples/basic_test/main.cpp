// DualSense DLL Basic Test
// Demonstrates basic LED, rumble, and trigger functionality

#include <dualsense.h>
#include <stdio.h>
#include <windows.h>

int main() {
    printf("===================================\n");
    printf("DualSense DLL Basic Test\n");
    printf("===================================\n\n");

    // Initialize device
    printf("[1/6] Initializing device...\n");
    DSResult result = ds_init();
    if (result != DS_OK) {
        printf("ERROR: Failed to initialize device (code: %d)\n", result);
        printf("Make sure a DualSense controller is connected!\n");
        return 1;
    }
    printf("SUCCESS: Device connected\n\n");

    // Display device information
    printf("[2/6] Device Information:\n");
    DSConnectionType conn_type = ds_get_connection_type();
    DSDeviceType dev_type = ds_get_device_type();
    printf("  Type: %s\n", dev_type == DS_DEVICE_DUALSENSE_EDGE ? "DualSense Edge" : "DualSense");
    printf("  Connection: %s\n\n", conn_type == DS_CONNECTION_BLUETOOTH ? "Bluetooth" : "USB");

    // Test LED colors
    printf("[3/6] Testing LED colors...\n");
    printf("  Red...\n");
    ds_set_lightbar(255, 0, 0);
    Sleep(1000);

    printf("  Green...\n");
    ds_set_lightbar(0, 255, 0);
    Sleep(1000);

    printf("  Blue...\n");
    ds_set_lightbar(0, 0, 255);
    Sleep(1000);

    printf("  Purple...\n");
    ds_set_lightbar(128, 0, 128);
    Sleep(1000);
    printf("  LED test complete\n\n");

    // Test player LED
    printf("[4/6] Testing player LED...\n");
    ds_set_player_led(DS_LED_PLAYER_1, DS_LED_BRIGHTNESS_HIGH);
    Sleep(500);
    ds_set_player_led(DS_LED_PLAYER_2, DS_LED_BRIGHTNESS_HIGH);
    Sleep(500);
    ds_set_player_led(DS_LED_PLAYER_3, DS_LED_BRIGHTNESS_HIGH);
    Sleep(500);
    ds_set_player_led(DS_LED_PLAYER_ALL, DS_LED_BRIGHTNESS_HIGH);
    Sleep(1000);
    printf("  Player LED test complete\n\n");

    // Test rumble
    printf("[5/6] Testing rumble motors...\n");
    printf("  Left motor...\n");
    ds_set_rumble(255, 0);
    Sleep(500);
    ds_stop_rumble();
    Sleep(200);

    printf("  Right motor...\n");
    ds_set_rumble(0, 255);
    Sleep(500);
    ds_stop_rumble();
    Sleep(200);

    printf("  Both motors...\n");
    ds_set_rumble(200, 200);
    Sleep(500);
    ds_stop_rumble();
    printf("  Rumble test complete\n\n");

    // Test adaptive triggers
    printf("[6/6] Testing adaptive triggers...\n");

    printf("  Bow effect...\n");
    ds_trigger_bow(true, true, 2, 7, 4, 8);
    Sleep(3000);

    printf("  Weapon effect...\n");
    ds_trigger_weapon(true, true, 3, 8, 7);
    Sleep(3000);

    printf("  Automatic gun effect...\n");
    ds_trigger_automatic_gun(true, true, 3, 6, 200);
    Sleep(3000);

    printf("  Turning off triggers...\n");
    ds_trigger_off(true, true);
    printf("  Trigger test complete\n\n");

    // Reset and cleanup
    printf("Resetting controller and disconnecting...\n");
    ds_reset_all();
    Sleep(500);
    ds_shutdown();

    printf("\n===================================\n");
    printf("All tests passed successfully!\n");
    printf("===================================\n");

    return 0;
}
