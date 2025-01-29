
#ifndef ADC_NTC_H
#define ADC_NTC_H

//include libraries
#include "driver/adc.h"
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "hal/adc_types.h"
#include "esp_adc/adc_oneshot.h" 
#include "math.h"

// ADC configuration
// ADC attenuation level
#define ADC1_ATTEN ADC_ATTEN_DB_12
// ADC channel
#define ADC1_CHAN ADC_CHANNEL_6
// Tag for the ADC
#define TAG "ADC_NTC"
// Queue for the temperature
extern QueueHandle_t TEMP_Q;

/**
 * @brief Configures the ADC with the specified attenuation and channel.
 *
 * This function initializes the ADC with the given attenuation level and channel.
 * It returns a handle to the ADC unit that can be used for further ADC operations.
 *
 * @param atten The attenuation level for the ADC.
 * @param channel The ADC channel to be configured.
 * @return adc_oneshot_unit_handle_t Handle to the configured ADC unit.
 */

// adc_config function declaration
adc_oneshot_unit_handle_t adc_config(adc_atten_t atten, adc_channel_t channel);

// adc_calibration_init function declaration
bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle);

// read_ntc function declaration
void read_ntc(void);

#endif // ADC_LIB_H
