#ifndef LED_RGB_H
#define LED_RGB_H

#include "driver/ledc.h"
#include <stdint.h>
#include "esp_err.h"

// Defined configuration for timer LEDC
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_HIGH_SPEED_MODE
#define LEDC_DUTY_RES           LEDC_TIMER_8_BIT // Resolución de 8 bits (0-255)
#define LEDC_FREQUENCY          (4000) // Frecuencia en Hertz (4 kHz)

// Defined configuration for LED RGB
#define LEDC_OUTPUT_IO_R        (18) // GPIO para el canal rojo
#define LEDC_OUTPUT_IO_G        (19) // GPIO para el canal verde
#define LEDC_OUTPUT_IO_B        (21) // GPIO para el canal azul
#define LEDC_CHANNEL_R          LEDC_CHANNEL_0
#define LEDC_CHANNEL_G          LEDC_CHANNEL_1
#define LEDC_CHANNEL_B          LEDC_CHANNEL_2

// Declared functions

/**
 * @brief Configure LEDC channels to control RGB LEDs.
 */
void configure_led_rgb(void);

/**
 * @brief Sets the color of the RGB LED with specific values for red, green, and blue.
 * 
 * @param red   Brightness of red (0-100)%.
 * @param green Brightness of green (0-100)%.
 * @param blue  Brightness of blue (0-100)%.
 */
void set_rgb_color(uint8_t red, uint8_t green, uint8_t blue);

#endif // LED_RGB_H

