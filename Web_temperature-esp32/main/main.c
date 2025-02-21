/**
 * Author: Javier Leonardo Guzmán Olaya
 * 
 * 
 */

// Include the necessary libraries
#include "nvs_flash.h"
#include "driver/gpio.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_log.h"
// #include "esp_wifi.h"
// #include "esp_event.h"



// Include the necessary libraries
#include "Librerias/Wifi_lib/wifi_app.h"
#include "Librerias/Adc_lib/ADC_NTC_POT.h"
#include "Librerias/Uart_lib/COMANDS_UART.h"
#include "Librerias/server_lib/http_server.h"
#include "Librerias/rgb_lib/rgb_led.h"

// Include the necessary libraries
#include "tasks_common.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_timer.h"
#include "sys/param.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc/adc_continuous.h"


// Defined channel adc to read the NTC and potentiometer
#define channel1 ADC1_CHANNEL_6 // NTC (GPIO34)
#define channel2 ADC1_CHANNEL_4// Potentiometer (GPIO32)



// Queues for the limits of the RGB LED
QueueHandle_t Lim_min_red;
QueueHandle_t Lim_max_red;
QueueHandle_t Lim_min_green;
QueueHandle_t Lim_max_green;
QueueHandle_t Lim_min_blue;
QueueHandle_t Lim_max_blue;

/**
 * 
 * @brief configuration led esp32
 * 
 */
static void configure_led(void)
{
    gpio_reset_pin(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
	rgb_led_pwm_init();
}


void app_main(void)
{
    // Initialize NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	// Led_configured
	configure_led();
	// Start Wifi
	wifi_app_start();

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
	// Create the queue for the potentiometer
	POT_V= xQueueCreate(10, sizeof(int));

	/*   Create the tasks         */ 
	// Create the task for uart
	xTaskCreatePinnedToCore(uart_task, "uart_task", 4096, NULL, 5, NULL,1);
	// Init ntc and pot and read the values
	read_ntc_pot(channel1,channel2);
	

	while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}

