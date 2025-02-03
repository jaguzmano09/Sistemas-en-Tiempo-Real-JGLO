#include "ADC_library.h"

/**
 * @brief Initializes the specified ADC unit.
 *
 * This function initializes the ADC unit specified by the parameter `adc_unit`.
 * It validates the ADC unit, checks if it has already been configured, and if not,
 * it configures the ADC unit and returns the configuration handle.
 *
 * @param adc_unit The ADC unit to initialize. Must be either `ADC_UNIT_1` or `ADC_UNIT_2`.
 * @return A configuration handle for the initialized ADC unit.
 */
config_unit init_adc(adc_unit_t adc_unit) {
    // Validate the ADC unit
    if (adc_unit != ADC_UNIT_1 && adc_unit != ADC_UNIT_2) {
        printf("Invalid ADC unit specified.\n");
    }
    
    // Static variables to store the ADC handles
    static adc_oneshot_unit_handle_t adc_handle_1= NULL;
    static adc_oneshot_unit_handle_t adc_handle_2 = NULL;
    adc_oneshot_unit_handle_t *adc_handle = NULL;

    // Select the ADC handle based on the specified ADC unit
    if (adc_unit == ADC_UNIT_1) {
        adc_handle = &adc_handle_1;
    } else if (adc_unit == ADC_UNIT_2) {
        adc_handle = &adc_handle_2;
    }

    // Check if the ADC handle has already been initialized
    if (*adc_handle != NULL) {
        config_unit config_unit_adc = {
            .adc_unit = adc_unit,
            .adc_handle = *adc_handle
        };
        return config_unit_adc;
    }

    // Initialize the ADC unit
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = adc_unit,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, adc_handle));

    // Configure the ADC unit
    config_unit config_unit_adc = {
        .adc_unit = adc_unit,
        .adc_handle = *adc_handle
    };

    return config_unit_adc;
}

/**
 * @brief Initializes the ADC channel with the specified configuration.
 *
 * This function initializes the ADC channel using the provided configuration.
 * It sets the attenuation and bit width for the ADC channel and associates it
 * with the initialized ADC unit.
 *
 * @param adc_config Pointer to the ADC configuration structure.
 * @param init_adc The configuration handle for the initialized ADC unit.
 * @return ESP_OK on success, or an error code on failure.
 */
esp_err_t init_adc_ch(adc_config_t *adc_config, config_unit init_adc){
     //-------------ADC Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .atten = adc_config->attenuation,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    adc_config->handle = init_adc.adc_handle ;
    adc_config->unit = init_adc.adc_unit ;


    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_config->handle , adc_config->channel, &config));

    //-------------ADC Calibration Init---------------//
    adc_config->cali_handle = NULL;
    adc_config->do_calibration = example_adc_calibration_init(adc_config->unit, adc_config->channel, adc_config->attenuation, &(adc_config->cali_handle));
    return ESP_OK;
}

// Read the raw ADC value from the potentiometer
esp_err_t read_adc_raw(adc_config_t *adc_config, int *adc_raw_value) {
    if (adc_config->handle == NULL) {
        printf("ADC handle is not initialized.\n");
        return ESP_ERR_INVALID_STATE;
    }
    return adc_oneshot_read(adc_config->handle, adc_config->channel, adc_raw_value);   
}

// Read the voltage value obtained from the potentiometer
esp_err_t read_voltage(adc_config_t *adc_config, int adc_raw_value, int *adc_voltage) {
    if (adc_config->do_calibration) {
        return adc_cali_raw_to_voltage(adc_config->cali_handle, adc_raw_value, adc_voltage);
    } else {
        return ESP_ERR_INVALID_STATE;
    }
}
// Deinitialize the ADC unit
esp_err_t deinit_adc(adc_config_t *adc_config){
    adc_oneshot_del_unit(adc_config->handle);
    if (adc_config->do_calibration) {
        example_adc_calibration_deinit(adc_config->cali_handle);
    }
    return ESP_OK;
}

/*---------------------------------------------------------------
        ADC Calibration
---------------------------------------------------------------*/
bool example_adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI(ADC_LIBRARY, "calibration scheme version is %s", "Curve Fitting");
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = unit,
            .chan = channel,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI("ADC_LIBRARY", "calibration scheme version is %s", "Line Fitting");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

    *out_handle = handle;
    if (ret == ESP_OK) {
        ESP_LOGI("ADC_LIBRARY", "Calibration Success");
    } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
        ESP_LOGW("ADC_LIBRARY", "eFuse not burnt, skip software calibration");
    } else {
        ESP_LOGE("ADC_LIBRARY", "Invalid arg or no memory");
    }

    return calibrated;
}

/**
 * @brief Deinitializes the specified calibration handle.
 *
 * This function deinitializes the calibration handle specified by the parameter `handle`.
 * It deregisters the calibration scheme and deletes the calibration handle.
 *
 * @param handle The calibration handle to deinitialize.
 */
void example_adc_calibration_deinit(adc_cali_handle_t handle)
{
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    ESP_LOGI(TAG, "deregister %s calibration scheme", "Curve Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(handle));

#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    ESP_LOGI("Main", "deregister %s calibration scheme", "Line Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(handle));
#endif
}

