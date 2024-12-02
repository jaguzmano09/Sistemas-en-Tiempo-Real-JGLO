// Author: Javier Leonardo Guzmán Olaya
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <driver/gpio.h>
#include <esp_system.h>
#include "sdkconfig.h"
#include "rom/ets_sys.h"
#include "esp_timer.h"

// Define the GPIO pins
#define BLINK_GPIO 2
#define BLINK_GPIO2 4
#define GPIO_OUTPUT_PIN_IN  ((1ULL<<GPIO_NUM_0))
#define GPIO_OUTPUT_PIN_OUT  ((1ULL<<GPIO_NUM_2))


// Define the flag status
typedef enum{
    FLAG_DEACTIVE,
    FLAG_ACTIVE,
}flag_status;

// Define the led status
typedef enum{
    LED_OFF,
    LED_ON,
}led_status;

flag_status button_flag;

// Configure the GPIO pins
void gpio_config_func()
{
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_IN ;
    //disable pull-down mode
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    //disable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE  ;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

   
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_OUT ;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
}

// Set the button flag to active when the button is pressed
void read_button()
{
    while (1)
    {   
        // Check if the button is pressed
        if(gpio_get_level(GPIO_NUM_0) == 0){
        button_flag = FLAG_ACTIVE; // Set the button flag to active
        }
        // Wait until the button is released
        while(gpio_get_level(GPIO_NUM_0) == 0){
            vTaskDelay(100);
        }
    }
    // Wait for 10ms
    vTaskDelay(10);

    }

// Change the state of the LED when the button flag is active
void change_led_state()
{
    // Initialize the status of the LED
    led_status status_led;
    // Set the status of the LED to off
    status_led = LED_OFF;
    while (1)
    {   
        // Check if the button flag is active
        if (button_flag == FLAG_ACTIVE){
            button_flag= FLAG_DEACTIVE; // Set the button flag to deactive
            // Check the status of the LED
            if (status_led == LED_ON){
                gpio_set_level(GPIO_NUM_2, 0); // Set the LED to off
                status_led = LED_OFF; // Set the status of the LED to off
            }
            else if (status_led == LED_OFF){
                gpio_set_level(GPIO_NUM_2, 1); // Set the LED to on
                status_led = LED_ON; // Set the status of the LED to on
            }
    }
    vTaskDelay(10);
    }
}

// Main function
void app_main(void)
{
    // Initialize gpio configuration
    gpio_config_func();
    // Create task for reading button
    xTaskCreatePinnedToCore(&read_button, "read_button", 2048, NULL, 5, NULL,0);
    // Create task for changing led state
    xTaskCreatePinnedToCore(&change_led_state, "change_led_state", 2048, NULL, 5, NULL,0);
}