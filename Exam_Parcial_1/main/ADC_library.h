// Include file for ADC_library.c
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

/**
 * @brief Configuration structure for the ADC
 * 
 * This structure defines the configuration for the ADC.
 * It is used to specify the ADC unit, channel, attenuation, and calibration handle.
 */
typedef struct {
    adc_unit_t unit;
    adc_channel_t channel;
    adc_atten_t attenuation;
    adc_oneshot_unit_handle_t handle;
    adc_cali_handle_t cali_handle;
    bool do_calibration;
} adc_config_t; 

/**
 * @brief Configuration structure for the ADC unit
 * 
 * This structure defines the configuration for the ADC unit.
 * It is used to specify the ADC handle and unit.
 */
typedef struct {
    adc_oneshot_unit_handle_t adc_handle;
    adc_unit_t adc_unit;
} config_unit;



config_unit init_adc(adc_unit_t adc_unit);
esp_err_t init_adc_ch(adc_config_t *adc_config, config_unit init_adc);
esp_err_t read_adc_raw(adc_config_t *adc_config, int *adc_raw_value);
esp_err_t read_voltage(adc_config_t *adc_config, int adc_raw_value, int *adc_voltage);
esp_err_t deinit_adc(adc_config_t *adc_config);
bool example_adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle);
void example_adc_calibration_deinit(adc_cali_handle_t handle);

