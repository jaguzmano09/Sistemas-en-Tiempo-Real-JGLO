#include "esp_err.h"
#include "driver/ledc.h"

// Definiciones globales para PWM
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Resolución de duty de 13 bits
#define LEDC_FREQUENCY          (4000)           // Frecuencia PWM de 4 kHz

// Estructura para representar un LED
typedef struct {
    int gpio_num;                   // Número de pin GPIO conectado al LED
    ledc_channel_t channel;         // Canal del controlador LED
    uint32_t duty;                  // Ciclo de trabajo inicial (duty)
} led;

// Estructura para representar un LED RGB
typedef struct {
    led led_red;                    // LED rojo
    led led_green;                  // LED verde
    led led_blue;                   // LED azul
} led_RGB;

// Declaraciones de funciones públicas

/**
 * @brief Inicializa el temporizador para un LED.
 * 
 * @return esp_err_t ESP_OK si se inicializó correctamente, otro valor en caso de error.
 */
esp_err_t ledc_init_timer(void);

/**
 * @brief Configura el canal del controlador LEDC para un LED.
 * 
 * @param l Estructura 'led' que contiene la configuración del LED.
 * @return esp_err_t ESP_OK si se configuró correctamente, otro valor en caso de error.
 */
esp_err_t  ledc_init_channel(led l);


/**
 * @brief Inicializa los canales LEDC para los LEDs de un LED RGB.
 * 
 * @param rgb Estructura `led_RGB` que contiene la configuración de los LEDs RGB.
 * @return esp_err_t ESP_OK si los canales se inicializaron correctamente, otro valor en caso de error.
 */
esp_err_t ledc_initialize_rgb(led_RGB rgb);


/**
 * @brief Convierte un porcentaje de duty en el valor correspondiente al contador de LEDC.
 * 
 * @param duty_porcent Porcentaje de duty (0-100%).
 * @return uint32_t Valor escalado para el controlador LEDC.
 */
uint32_t set_duty_porcent(uint32_t duty_porcent);

/**
 * @brief Configura el duty cycle para un LED específico.
 * @param l Estructura `led` que contiene la configuración del LED (GPIO, canal, etc.).
 * @param duty Ciclo de trabajo deseado en porcentaje (0-100). 
 *             Si se proporciona un valor fuera de este rango, se retorna un error.
 * @return
 *      - ESP_OK: Si el duty cycle se configuró correctamente.
 *      - ESP_ERR_INVALID_ARG: Si el duty cycle está fuera del rango permitido.
 *      - Otros códigos de error definidos por LEDC en caso de fallos en la configuración.
 */
esp_err_t set_led_color(led l, uint32_t duty);

/**
 * @brief Establece el color del LED RGB mediante los ciclos de trabajo (duty).
 * 
 * @param rgb Estructura `led_RGB` que representa los LEDs RGB.
 * @param duty_red Ciclo de trabajo para el LED rojo (0-100%).
 * @param duty_green Ciclo de trabajo para el LED verde (0-100%).
 * @param duty_blue Ciclo de trabajo para el LED azul (0-100%).
 * @return esp_err_t ESP_OK si los valores se configuraron correctamente, otro valor en caso de error.
 */
esp_err_t rgb_set_color(led_RGB rgb, uint32_t duty_red, uint32_t duty_green, uint32_t duty_blue);


