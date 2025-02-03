#include "ADC_NTC.h"

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
adc_oneshot_unit_handle_t adc_config( adc_atten_t atten,adc_channel_t channel ){
    adc_oneshot_unit_handle_t handle;//ADC handle
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };//ADC configuration

    // Initialize ADC unit
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &handle));//ADC initialization check

    //-------------ADC Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = atten,
    };//ADC configuration
    ESP_ERROR_CHECK(adc_oneshot_config_channel(handle, channel, &config));//ADC configuration check
    return handle;
}

/**
 * @brief Initializes ADC calibration.
 *
 * This function initializes the ADC calibration with the specified unit, channel, and attenuation.
 * It returns a handle to the calibration unit that can be used for further calibration operations.
 *
 * @param unit The ADC unit to be calibrated.
 * @param channel The ADC channel to be calibrated.
 * @param atten The attenuation level for the ADC.
 * @param out_handle Pointer to the handle to be returned for the calibration unit.
 * @return bool True if calibration was successful, false otherwise.
 */

bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;//ADC calibration handle
    esp_err_t ret = ESP_FAIL;//Error check
    bool calibrated = false;//Calibration check
    if (!calibrated) {
        // Line Fitting calibration
        ESP_LOGI(TAG, "calibration scheme version is %s", "Line Fitting");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        // Create calibration scheme
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            // Run calibration
            calibrated = true;
        }
    }

    *out_handle = handle;//Return the calibration handle
    if (ret == ESP_OK) {
        // Calibration success
        ESP_LOGI(TAG, "Calibration Success");
    } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
        // Calibration not supported
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    } else {
        // Calibration failed
        ESP_LOGE(TAG, "Invalid arg or no memory");
    }

    // Return calibration status
    return calibrated;
}

/** @brief Read the NTC temperature 
 * 
 * This function reads the NTC temperature and sends it to a queue. 
 */ 

void read_ntc(){
    int adc_value[1][1];//ADC value
    int voltage_value[1][1];//Voltage value
    int temperature=0;//Temperature value

    //ADC read configuration
    adc_oneshot_unit_handle_t adc1_handle=adc_config(ADC1_ATTEN,ADC1_CHAN);
    //ADC calibration configuration
    adc_cali_handle_t adc1_cali_chan0_handle = NULL;
    //ADC calibration initialization
    bool do_calibration1_chan0 = adc_calibration_init(ADC_UNIT_1, ADC1_CHAN, ADC1_ATTEN, &adc1_cali_chan0_handle);
    
    // Read the NTC temperature
    for(;;){
        //ADC read
        adc_oneshot_read(adc1_handle, ADC1_CHAN, &adc_value[0][0]);
        //ADC calibration
        if(do_calibration1_chan0){
        // Convert the raw ADC value to voltage
        adc_cali_raw_to_voltage(adc1_cali_chan0_handle, adc_value[0][0], &voltage_value[0][0]);}
        // Calculate the temperature
        temperature = (-0.033324*voltage_value[0][0])+78.22;
        // Print the NTC voltage and temperature
        printf("NTC_voltaje: %d\n",voltage_value[0][0]);
        printf("Temperature: %d\n", temperature);
        // Send the temperature to the queue
        xQueueSend( TEMP_Q , &temperature , pdMS_TO_TICKS(0));
        // Delay
        vTaskDelay(100);
    }

}