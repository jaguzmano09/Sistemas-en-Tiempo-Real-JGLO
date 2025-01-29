// Author: Javier Leonardo Guzmán Olaya
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "LED_RGB.h"
#include "ADC_NTC.h"
#include "freertos/queue.h"
#include "COMANDS_UART.h"
#include "freertos/semphr.h"


#define LEDC_DUTY_RES           LEDC_TIMER_8_BIT // Resolution
// GPIO for RGB LED
#define GPIO_RED                18 // GPIO for red channel
#define GPIO_GREEN              19 // GPIO for green channel
#define GPIO_BLUE               21 // GPIO for blue channel

// Queues for the limits of the RGB LED
QueueHandle_t Lim_min_red;
QueueHandle_t Lim_max_red;
QueueHandle_t Lim_min_green;
QueueHandle_t Lim_max_green;
QueueHandle_t Lim_min_blue;
QueueHandle_t Lim_max_blue;

// Define the configuration for the button
#define GPIO_NUM                0 // GPIO for button
#define GPIO_OUTPUT_PIN_IN  ((1ULL<<GPIO_NUM_0)) // Input pin
#define GPIO_OUTPUT_PIN_OUT  ((1ULL<<GPIO_NUM_2)) // Output pin
#define BUTTON_PRESSED 1 // Button pressed
#define BUTTON_RELEASED 0 // Button released

// Define the configuration for the button
typedef enum{
        RED_Button,
        GREEN_Button,
        BLUE_Button,
        All_Button,
}Button_conf; 

// Define queue for button state
QueueHandle_t buttonQueue;
// Define queue for button configuration color RGB
QueueHandle_t ConfQueue;

/*          GPIO CONFIGURATION          */
/**
 * @brief Function to configure the GPIO.
 */
void gpio_config_func()
{
    /*             GPIO    CONFIGURATIONS             */
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
}

/*          READ STATE BUTTON          */
/**
 * @brief Task to read the state of the button.
 */
void read_button(void *arg)
{ 
    Button_conf button_conf = RED_Button;
    while (1)
    {
        if (gpio_get_level(GPIO_NUM_0)==0){
            xQueueReceive(ConfQueue, &button_conf,pdMS_TO_TICKS(100));
            if (button_conf==All_Button)
            {
                button_conf=RED_Button;
                xQueueSend(ConfQueue, &button_conf, pdMS_TO_TICKS(0));
            }
            else if (button_conf==RED_Button)
            {
                button_conf=GREEN_Button;
                xQueueSend(ConfQueue, &button_conf, pdMS_TO_TICKS(0));
            }
            else if(button_conf==GREEN_Button){
                button_conf=BLUE_Button;
                xQueueSend(ConfQueue, &button_conf, pdMS_TO_TICKS(0));
            }
            else if(button_conf==BLUE_Button){
                button_conf=All_Button;
                xQueueSend(ConfQueue, &button_conf, pdMS_TO_TICKS(0));
            }

            else{
                button_conf=All_Button;
                xQueueSend(ConfQueue, &button_conf, pdMS_TO_TICKS(0));
            }
            while (gpio_get_level(GPIO_NUM_0)==0){
                    vTaskDelay(10);
            }
    }
    vTaskDelay(10);
    }
}

/**
 * @brief Configures the color limits for the RGB LED.
 *
 * This function reads the minimum and maximum limits for each color channel
 * from their respective queues and configures the RGB LED accordingly.
 */
void colorconf(){
    // Variables for the limits of the RGB LED and the temperature
    int temp_;
    int red_min=0;
    int red_max=0;
    int green_min=0;
    int green_max=0;
    int blue_min=0;
    int blue_max=0;
    // Variables for the RGB LED colors
    int Red;
    int Green;
    int Blue;
    // Variable for the button configuration
    Button_conf button_conf;

    bool red_message_shown = false;
    bool green_message_shown = false;
    bool blue_message_shown = false;
    bool all_message_shown = false;


    while (1) {
        // Receive temperature from NTC
        xQueueReceive(TEMP_Q, &temp_, 100 / portTICK_PERIOD_MS);

        // Receive LED color limits from UART
        xQueueReceive(Lim_min_red, &red_min, 100 / portTICK_PERIOD_MS);
        xQueueReceive(Lim_max_red, &red_max, 100 / portTICK_PERIOD_MS);
        xQueueReceive(Lim_min_green, &green_min, 100 / portTICK_PERIOD_MS);
        xQueueReceive(Lim_max_green, &green_max, 100 / portTICK_PERIOD_MS);
        xQueueReceive(Lim_min_blue, &blue_min, 100 / portTICK_PERIOD_MS);
        xQueueReceive(Lim_max_blue, &blue_max, 100 / portTICK_PERIOD_MS);

        // Receive button configuration
        xQueueReceive(ConfQueue, &button_conf, pdMS_TO_TICKS(100));

        // Reset RGB values
        Red = 0;
        Green = 0;
        Blue = 0;

        char message[100];  // Buffer para almacenar el mensaje

        // Configure the RGB LED based on button pressed
        switch (button_conf) {
            // Configure the RED LED based on the temperature limits
            case RED_Button:
                if (temp_ >= red_min && temp_ <= red_max) {
                    Red = 100;// Brightness of red 100%
                    if (!red_message_shown) {
                        // Show message with temperature and limits
                        snprintf(message, sizeof(message), 
                                "\n RED LED ON | Temp: %d°C | Limits: %d-%d°C\n", 
                                temp_, red_min, red_max);
                        // Send message to UART
                        send_uart_response(message);
                        // Set flag to true
                        red_message_shown = true;
                    }
                } else {
                    // Set flag to false
                    red_message_shown = false;
                }
                break;
            // Configure the GREEN LED based on the temperature limits
            case GREEN_Button:
                if (temp_ >= green_min && temp_ <= green_max) {
                    Green = 100;// Brightness of green 100%
                    if (!green_message_shown) {
                        // Show message with temperature and limits
                        snprintf(message, sizeof(message), 
                                "\n GREEN LED ON | Temp: %d°C | Limits: %d-%d°C\n", 
                                temp_, green_min, green_max);
                        // Send message to UART
                        send_uart_response(message);
                        // Set flag to true
                        green_message_shown = true;
                    }
                } else {
                    // Set flag to false
                    green_message_shown = false;
                }
                break;
            // Configure the BLUE LED based on the temperature limits
            case BLUE_Button:
                if (temp_ >= blue_min && temp_ <= blue_max) {
                    Blue = 100;// Brightness of blue 100%
                    if (!blue_message_shown) {
                        // Show message with temperature and limits
                        snprintf(message, sizeof(message), 
                                "BLUE LED ON | Temp: %d°C | Limits: %d-%d°C\n", 
                                temp_, blue_min, blue_max);
                        // Send message to UART
                        send_uart_response(message);
                        // Set flag to true
                        blue_message_shown = true;
                    }
                } else {
                    // Set flag to false
                    blue_message_shown = false;
                }
                break;
            // Configure all LEDs based on the temperature limits
            case All_Button:
                if (!all_message_shown){
                    // Show message with temperature and limits
                    snprintf(message, sizeof(message),
                            "\n RGB Status | Temp: %d°C | R:%d-%d°C G:%d-%d°C B:%d-%d°C\n",
                            temp_, red_min, red_max, green_min, green_max, blue_min, blue_max);
                    // Send message to UART
                    send_uart_response(message);
                    // Set flag to true
                    all_message_shown = true;
                }
                break;
            default:
                // No button pressed, keep LEDs off
                break;
        }

        // Apply final color setting
        set_rgb_color(Red, Green, Blue, LEDC_DUTY_RES);

        // Delay for 100 ms
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

}

/**
 * @brief Main function.
 */
void app_main(void)
{
    // Initialize the RGB LED
    configure_led_rgb(GPIO_RED, GPIO_GREEN, GPIO_BLUE, LEDC_DUTY_RES);

    // Create the queues for the limits of the RGB LED
    Lim_min_red=xQueueCreate(10,sizeof(int));
    Lim_max_red=xQueueCreate(10,sizeof(int));
    Lim_min_green=xQueueCreate(10,sizeof(int));
    Lim_max_green=xQueueCreate(10,sizeof(int));
    Lim_min_blue=xQueueCreate(10,sizeof(int));
    Lim_max_blue=xQueueCreate(10,sizeof(int));
    // Create the queue for the temperature
    TEMP_Q= xQueueCreate(10, sizeof(int));
    // Create the queue for the button state
    buttonQueue = xQueueCreate(10, sizeof(int));
    // Create the queue for the button configuration
    ConfQueue = xQueueCreate(10, sizeof(Button_conf));

    /* Create the tasks */
    // Create the task for the color configuration
    xTaskCreatePinnedToCore(colorconf, "colorconf", 4096, NULL, 5, NULL, 1);
    // Create the task for reading the NTC
    xTaskCreatePinnedToCore(read_ntc, "read_ntc", 4096, NULL, 5, NULL, 1);
    // Create the task for the UART communication
    xTaskCreatePinnedToCore(uart_task, "uart_task", 4096, NULL, 5, NULL, 0);
    // Create the task for reading the button
    xTaskCreatePinnedToCore(read_button, "read_button", 4096, NULL, 5, NULL, 1);

}
