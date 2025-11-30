#include "dualsense.h"
#include <stdio.h>
#include <Windows.h>

int main() {
    printf("DualSense Touchpad Test\n");
    printf("Touch the touchpad to see coordinates.\n");
    printf("Press touchpad button to exit.\n\n");

    // Initialize
    if (ds_init() != DS_OK) {
        printf("Failed to initialize DualSense\n");
        return 1;
    }

    printf("Connected! Waiting for touchpad input...\n\n");

    DSInputState state;
    DSInputState prev_state = {};

    while (true) {
        // Update and read input
        ds_update_input();
        ds_get_input_state(&state);

        // Detect touch 1 events (client-side event detection)
        if (state.touch1.is_active && !prev_state.touch1.is_active) {
            printf("[Touch1 START] ID=%d, X=%4d, Y=%4d\n",
                   state.touch1.id, state.touch1.x, state.touch1.y);
        }
        else if (state.touch1.is_active && prev_state.touch1.is_active) {
            // Only print if position changed
            if (state.touch1.x != prev_state.touch1.x ||
                state.touch1.y != prev_state.touch1.y) {
                printf("[Touch1 MOVE ] ID=%d, X=%4d, Y=%4d\n",
                       state.touch1.id, state.touch1.x, state.touch1.y);
            }
        }
        else if (!state.touch1.is_active && prev_state.touch1.is_active) {
            printf("[Touch1 END  ] ID=%d\n", prev_state.touch1.id);
        }

        // Detect touch 2 events
        if (state.touch2.is_active && !prev_state.touch2.is_active) {
            printf("[Touch2 START] ID=%d, X=%4d, Y=%4d\n",
                   state.touch2.id, state.touch2.x, state.touch2.y);
        }
        else if (state.touch2.is_active && prev_state.touch2.is_active) {
            if (state.touch2.x != prev_state.touch2.x ||
                state.touch2.y != prev_state.touch2.y) {
                printf("[Touch2 MOVE ] ID=%d, X=%4d, Y=%4d\n",
                       state.touch2.id, state.touch2.x, state.touch2.y);
            }
        }
        else if (!state.touch2.is_active && prev_state.touch2.is_active) {
            printf("[Touch2 END  ] ID=%d\n", prev_state.touch2.id);
        }

        // Check touchpad button
        if (state.button_touchpad) {
            printf("\nTouchpad button pressed - exiting\n");
            break;
        }

        prev_state = state;
        Sleep(16);  // ~60 FPS polling rate
    }

    ds_shutdown();
    return 0;
}
