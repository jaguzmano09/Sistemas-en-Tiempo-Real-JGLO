// Author: Javier Leonardo Guzmán Olaya
#include <stdio.h>
#include "LED_RGB.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LEDC_DUTY_RES           LEDC_TIMER_8_BIT // Resolution
// GPIO for RGB LED
#define GPIO_RED                18 // GPIO for red channel
#define GPIO_GREEN              19 // GPIO for green channel
#define GPIO_BLUE               21 // GPIO for blue channel
// Main function
void app_main() {
    // Configure the RGB LED
    configure_led_rgb(GPIO_RED, GPIO_GREEN, GPIO_BLUE, LEDC_DUTY_RES);
    
    // Infinite loop
    while (1) {
        // Change the color to red
        set_rgb_color(100, 0, 0, LEDC_DUTY_RES);
        vTaskDelay(100);

        // Change the color to green
        set_rgb_color(0, 100, 0, LEDC_DUTY_RES);
        vTaskDelay(100);

        // Change the color to blue
        set_rgb_color(0, 0, 100, LEDC_DUTY_RES);
        vTaskDelay(100);

        // Change the color to white
        set_rgb_color(100, 100, 100,LEDC_DUTY_RES);
        vTaskDelay(100);
    }
}