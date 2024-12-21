// Author: Javier Leonardo Guzmán Olaya

// This code reads the state of a button and the value of a potentiometer using the ADC and change state led RGB.

//Include the required headers
#include <string.h>
#include "LED_RGB.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_adc/adc_oneshot.h"
#include "hal/adc_types.h"

#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_cali.h"

#include "freertos/queue.h"
#include "freertos/semphr.h"

// Define the configuration for the LED RGB
#define LEDC_DUTY_RES           LEDC_TIMER_9_BIT // Resolution
#define GPIO_RED                18 // GPIO for red channel
#define GPIO_GREEN              19 // GPIO for green channel
#define GPIO_BLUE               21 // GPIO for blue channel
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

/*          ADJUST LED BRIGHTNESS          */
void adjust_led_brightness(void *arg){
    // Initialize variables
    int potentiometer_value=0;
    int potentiometer_out = 0;
    int glow = 0;
    int Brightness_RED = 0;
    int Brightness_GREEN = 0;
    int Brightness_BLUE = 0;
    Button_conf button_conf;

    /*                        CREATE ADC UNIT                           */
    adc_oneshot_unit_handle_t adc1_handle;//ADC1
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,//ADC1
        .ulp_mode = ADC_ULP_MODE_DISABLE,//Disable ULP
    };
    // Create ADC unit
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));
    //-------------ADC1 Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_9,//9 bits
        .atten = ADC_ATTEN_DB_12,//Attenuation 12 dB
    };
    // Configure ADC channel
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle,ADC_CHANNEL_1, &config));

    /*                          CALIBRATION                             */
    adc_cali_handle_t cali_handle = NULL;//Calibration handle
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,//ADC1
        .atten = ADC_ATTEN_DB_6,//Attenuation 6 dB
        .bitwidth = ADC_BITWIDTH_9,//9 bits
    };
    // Create calibration scheme
    ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&cali_config, &cali_handle));

    /*              READ ADC AND CHANGE COLOR LED RGB                                 */
    /**
        * @brief Main loop to read the ADC and adjust the RGB LED color.
        * In this loop, the ADC value is read, converted to voltage, and the brightness
        * of the RGB LED is adjusted based on the read value. The button state is also
        * read to determine which color to adjust.
        */
    // Main loop
    while(1)
    {
        // Read ADC value
        adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &potentiometer_value);
        // Convert ADC value to voltage
        adc_cali_raw_to_voltage(cali_handle, potentiometer_value, &potentiometer_out);
        // Print ADC value in mV
        ESP_LOGI("ADC", "ADC Value: %d mV", potentiometer_out);
        // Convert ADC value to brightness value
        glow = (potentiometer_value* 100) / 4095;
        // Receive button state
        xQueueReceive(ConfQueue, &button_conf, pdMS_TO_TICKS(100));
        // Adjust LED brightness based on button state
        switch (button_conf) {
            case RED_Button:
                // Adjust RED color
                Brightness_RED = glow;//Adjust brightness
                set_rgb_color(Brightness_RED, 0, 0, LEDC_DUTY_RES);//Set color
                printf("Brightness RED: %d\n",Brightness_RED);//Print brightness
                break;

            case GREEN_Button:
                // Adjust GREEN color
                Brightness_GREEN = glow; //Adjust brightness
                set_rgb_color(0, Brightness_GREEN, 0, LEDC_DUTY_RES);//Set color
                printf("Brightness GREEN: %d \n",Brightness_GREEN);//Print brightness
                break;

            case BLUE_Button:
                // Adjust BLUE color
                Brightness_BLUE = glow;//Adjust brightness
                set_rgb_color(0, 0, Brightness_BLUE, LEDC_DUTY_RES);//Set color
                printf("Brightness BLUE: %d \n",Brightness_BLUE);//Print brightness
                break;

            case All_Button:
                // Adjust all colors
                set_rgb_color(Brightness_RED, Brightness_GREEN, Brightness_BLUE, LEDC_DUTY_RES);//Set color
                printf("Brightness: RED:%d, GREEN:%d, BLUE:%d \n",Brightness_RED, Brightness_GREEN, Brightness_BLUE);//Print brightness
                break;
            default:
                // Turn off LED
                set_rgb_color(0, 0, 0, LEDC_DUTY_RES);//Set color
                printf("Brightness: RED:%d, GREEN:%d, BLUE:%d \n",Brightness_RED, Brightness_GREEN, Brightness_BLUE);//Print brightness
                break;
        }
        // Wait for 100 ms
        vTaskDelay(100);

    }
}

/*         MAIN FUNCTION         */
/**
 * @brief Main function to initialize the GPIO and create tasks.
 */
void app_main(void){

    // Initialize gpio configuration
    gpio_config_func();
   // Configure RGB LED
    configure_led_rgb(GPIO_RED, GPIO_GREEN, GPIO_BLUE, LEDC_DUTY_RES);
    // Create queue for button state
    buttonQueue = xQueueCreate(20, sizeof(uint32_t));
    if (buttonQueue == NULL)
    {
        printf("Failed to create queue\n");
        return;
    }
    // Create queue for button configuration color RGB
    ConfQueue = xQueueCreate(20, sizeof(uint32_t));
    if (ConfQueue == NULL)
    {
        printf("Failed to create queue\n");
        return;
    }
    // Create task for reading button
    xTaskCreatePinnedToCore(read_button, "read_button", 2048, NULL, 5, NULL, 0);
    // Create task for reading ADC
    xTaskCreatePinnedToCore(adjust_led_brightness, "adjust_led_brightness", 4096, NULL, 4, NULL, 0);
}