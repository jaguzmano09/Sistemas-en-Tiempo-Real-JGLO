// Author: Javier Leonardo Guzmán Olaya
#include <stdio.h>
#include "LED_RGB.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Main function
void app_main() {
    // Configure the RGB LED
    configure_led_rgb();
    
    // Infinite loop
    while (1) {
        // Change the color to red
        set_rgb_color(25, 0, 0);
        vTaskDelay(100);

        // Change the color to green
        set_rgb_color(0, 50, 0);
        vTaskDelay(100);

        // Change the color to blue
        set_rgb_color(0, 0, 75);
        vTaskDelay(100);

        // Change the color to white
        set_rgb_color(100, 100, 100);
        vTaskDelay(100);
    }
}