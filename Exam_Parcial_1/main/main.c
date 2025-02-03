// Authors: Javier Leonardo Guzmán Olaya, Brayan Avendaño

// Included libraries
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "LED_RGB.h"
#include "ADC_NTC.h"
#include "freertos/queue.h"
#include "COMANDS_UART.h"
#include "freertos/semphr.h"
#include "ADC_library.h"
#include "led_RGB_library.h"

// Defined pins for the RGB LED 2 (potentiometer)
#define LEDC_OUTPUT_IO_RED_POT       (13)   // GPIO para el LED rojo
#define LEDC_OUTPUT_IO_GREEN_POT     (12)  // GPIO para el LED verde
#define LEDC_OUTPUT_IO_BLUE_POT      (14)   // GPIO para el LED azul

// Defined pins for the RGB LED 2 (potentiometer)
#define LEDC_DUTY_INITAL          (0)   // Duty inicial (apagado)Set duty to 50%. (2 ** 13) * 50% = 4096

// GPIO for RGB LED 1 (NTC)
#define GPIO_RED                18 // GPIO for red channel
#define GPIO_GREEN              19 // GPIO for green channel
#define GPIO_BLUE               21 // GPIO for blue channel

// Defined pins for button
#define GPIO_INPUT_IO_0     0 // GPIO for button 1
#define GPIO_INPUT_IO_1     5 // GPIO for button 2
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0) | (1ULL<<GPIO_INPUT_IO_1)) // GPIO for button 1 and 2

// 
#define ESP_INTR_FLAG_DEFAULT 0

// Queues for the limits of the RGB LED
QueueHandle_t Lim_min_red;
QueueHandle_t Lim_max_red;
QueueHandle_t Lim_min_green;
QueueHandle_t Lim_max_green;
QueueHandle_t Lim_min_blue;
QueueHandle_t Lim_max_blue;
QueueHandle_t TEMP_ONOFF;

// 
static int adc_raw[2][10];
static int voltage[2][10];



// 
typedef enum {
    RED = 0,
    GREEN,
    BLUE
} color_t;

static uint8_t current_color = RED; // Current color of the RGB LED




/**
 * @brief Configuration of ADC2 channel 0.
 *
 * This structure defines the configuration for channel 0 of the ADC2 (Analogue-to-Digital Converter).
 * It is used to specify parameters such as channel, attenuation, calibration, etc.
 * This ADC is used to read the voltage value obtained from the potentiometer.
 */
adc_config_t adc2_config_ch0 = {
    .channel = ADC_CHANNEL_0, // ADC channel 0
    .attenuation = ADC_ATTEN_DB_12, // Attenuation level of 12 dB
    .cali_handle = NULL, // No calibration
    .do_calibration = false // No calibration
}; 

/**
 * @brief Configuration of structure for the RGB LED 2.
 * 
 * This structure defines the configuration for the RGB LED 2,
 * which works in conjunction with the potentiometer.
 * It is used to specify the GPIO number, LEDC channel, and duty cycle.
 * The duty cycle is set to 0% initially.
 */
led_RGB rgb_pot = {
        .led_red = {
            .gpio_num    = LEDC_OUTPUT_IO_RED_POT, // GPIO for the red LED
            .channel     = LEDC_CHANNEL_3, // LEDC channel 3
            .duty        = LEDC_DUTY_INITAL // Duty cycle of 0%
        },
        .led_green = {
            .gpio_num    = LEDC_OUTPUT_IO_GREEN_POT, // GPIO for the green LED
            .channel     = LEDC_CHANNEL_4, // LEDC channel 4
            .duty        = LEDC_DUTY_INITAL // Duty cycle of 0%
        },
        .led_blue = {
            .gpio_num    = LEDC_OUTPUT_IO_BLUE_POT, // GPIO for the blue LED
            .channel     = LEDC_CHANNEL_5, // LEDC channel 5
            .duty        = LEDC_DUTY_INITAL // Duty cycle of 0%
        }
};


/**
 * @brief Initalized the RGB LED 2.
 * 
 * This function initializes the RGB LED 2, which works in conjunction with the potentiometer.
 * It sets the GPIO pins for the red, green, and blue LEDs, and initializes the timers for each LED.
 */
static void rgb_init(led_RGB rgb) {
    // Initialize the timers for the RGB LED
    ledc_init_timer();
    ledc_initialize_rgb(rgb);
};



/**
 * @brief Configures the ADC unit.
 * 
 * This function configures the ADC unit with the specified channel and attenuation level.
 * It initializes the ADC unit and the ADC channel.
 * 
 * @param acd_ch The configuration structure for the ADC channel.
 * @param adc_unit The ADC unit to be configured.
 */
static void config_adc_unit(adc_config_t *acd_ch, adc_unit_t adc_unit ){
    config_unit init_adc_unit = init_adc(adc_unit);
    init_adc_ch(acd_ch, init_adc_unit);
}


/**
 * @brief ISR handler for the button.
 * 
 * This function is the ISR handler for the button.
 * It changes the current color of the RGB LED when the button is pressed.
 * 
 * @param arg The argument passed to the ISR handler.
 */
static void IRAM_ATTR gpio_isr_handler(void* arg)
{   //  Change the color of the RGB LED when the button is pressed
    current_color = (current_color + 1) % 3;
}

/**
 * @brief Configurited the gpio button.
 * 
 * This function configures the GPIO pins for the buttons.
 * It sets the pins as input mode and enables the pull-up mode.
 */
static void config_button(){
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_down_en = 1;
    gpio_config(&io_conf);

    //change gpio interrupt type for one pin
    gpio_set_intr_type(GPIO_INPUT_IO_0, GPIO_INTR_POSEDGE);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);

    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_1, gpio_isr_handler, (void*) GPIO_INPUT_IO_1);

    printf("Minimum free heap size: %"PRIu32" bytes\n", esp_get_minimum_free_heap_size());
}

/**
 * @brief Task for the button.
 * 
 * This function initializes the RGB LED with the potentiometer and the ADC unit.
 * It reads the raw ADC value from the potentiometer and calculates the duty cycle for the RGB LED.
 * The color of the LED is changed based on the current color.
 */
static void button_task(){
    // Init the RGB LED 2 
    rgb_init(rgb_pot);
    // Config the ADC unit
    config_adc_unit(&adc2_config_ch0, ADC_UNIT_2);

    // Variables for the raw ADC value and the duty cycle
    while (1)
    {
        // Read the raw ADC value from the potentiometer
        read_adc_raw(&adc2_config_ch0, &adc_raw[0][1]);
        // Calculate the duty cycle for the RGB LED
        int32_t duty = (100 * adc_raw[0][1])/4095; 
        /*    Set the color of the RGB LED based on the current color        */ 
        switch (current_color){
            case RED:
                // Set the color of the RGB LED to red
                rgb_set_color(rgb_pot, duty, 0, 0); // Red
                break;
            case GREEN:
                // Set the color of the RGB LED to green
                rgb_set_color(rgb_pot, 0, duty, 0); // Green
                break;
            case BLUE:
                // Set the color of the RGB LED to blue
                rgb_set_color(rgb_pot, 0, 0, duty); //  Blue
                break;
            default:
                break;
        }
        vTaskDelay(100/portTICK_PERIOD_MS);
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


        // Reset RGB values
        Red = 0;
        Green = 0;
        Blue = 0;

        char message[100];  // Buffer para almacenar el mensaje

        // Configure the RGB LED based on button pressed
 
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
        // Configure the GREEN LED based on the temperature limits
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
        // Configure the BLUE LED based on the temperature limits
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
        // Apply final color setting
        set_rgb_color(Red, Green, Blue, LEDC_DUTY_RES);
        }

        // Delay for 100 ms
        vTaskDelay(100 / portTICK_PERIOD_MS);
}


/**
 * @brief Task to obtain the NTC temperature
 * 
 * This function allows you to switch the temperature reading of the ntc off and on.
 */
void GET_TEMP_FUNCTION(){
    int status;
    char buffer[1025];
    int temp;   
    while (1)
    {
       xQueueReceive(TEMP_Q,&temp, 100/ portTICK_PERIOD_MS);
       xQueueReceive(TEMP_ONOFF,&status,100/ portTICK_PERIOD_MS);
        if (status!=NULL || status!=0){
            if (status == 1) {
                while (status == 1) {
                    snprintf(buffer, sizeof(buffer), "\n Temperatura: %d °C\n", temp);
                    send_uart_response(buffer);
                    xQueueReceive(TEMP_Q,&temp, 100/ portTICK_PERIOD_MS);
                    xQueueReceive(TEMP_ONOFF, &status, pdMS_TO_TICKS(100)); 
                    vTaskDelay(pdMS_TO_TICKS(2000));
                }
                send_uart_response("\n Temperatura apagada\n"); // Solo se envía una vez
            }     
        vTaskDelay(10);
        }
    }
           
}

/**
 * @brief Main Function.
 */
void app_main(void)
{
    // Initialize the button
    config_button();
    // Initialize the RGB LED 1 (NTC)
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
    TEMP_ONOFF= xQueueCreate(10, sizeof(int));

    /* Create the tasks */
    // Create the task for the color configuration
    xTaskCreatePinnedToCore(colorconf, "colorconf", 4096, NULL, 5, NULL, 1);
    // Create the task for reading the NTC
    xTaskCreatePinnedToCore(read_ntc, "read_ntc", 4096, NULL, 4, NULL, 1);
    // Create the task for the UART communication
    xTaskCreatePinnedToCore(uart_task, "uart_task", 4096, NULL, 5, NULL, 0);
    // Create the task for the button
    xTaskCreate(button_task, "button_task", 4096, NULL, configMAX_PRIORITIES -  3, NULL);
    // Create the task for the temperature
    xTaskCreate(GET_TEMP_FUNCTION, "GET_TEMP_FUNCTION", 4096, NULL, configMAX_PRIORITIES -  3, NULL);

    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}
