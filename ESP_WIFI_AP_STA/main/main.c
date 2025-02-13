// Author: Javier Guzmán Olaya

// This code is an example of how to configure the ESP32 as a station and as an access point at the same time.

// Include Libraries
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_mac.h"
#include "lwip/err.h"
#include "lwip/sys.h"

// Defined wifi configuration ap 
#define EXAMPLE_ESP_WIFI_SSID      "ESP_JAVIER_RED" // Name of the access point
#define EXAMPLE_ESP_WIFI_PASS      "12345678"       // Password of the access point
#define EXAMPLE_ESP_WIFI_CHANNEL    1               // Channel of the access point
#define EXAMPLE_MAX_STA_CONN        4               // Maximum number of stations that can connect to the access point
#define EXAMPLE_ESP_MAXIMUM_RETRY   10              // Maximum number of retries to connect to the access point

/*       Defined wifi configuration station                */ 
#ifndef ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD // If the threshold is not defined, it is defined as WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK // Threshold of the authentication mode
#endif

#ifndef ESP_WIFI_SAE_MODE // If the SAE mode is not defined, it is defined as WPA3_SAE_PWE_HUNT_AND_PECK
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_HUNT_AND_PECK // SAE mode
#endif

#ifndef EXAMPLE_H2E_IDENTIFIER // If the H2E identifier is not defined, it is defined as my_h2e_identifier
#define EXAMPLE_H2E_IDENTIFIER "my_h2e_identifier" // H2E identifier
#endif


/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

// Function to handle the events of the wifi
static const char *TAG = "Wifi AP_STA";

// Number of retries to connect to the access point
static int s_retry_num = 0;


/**
 * @brief Event handler for catching system events
 * 
 * @param arg : Argument
 * @param event_base : Event base
 * @param event_id : Event id
 * @param event_data : Event data
 * 
 * @return void
 * 
 * This function is used to handle the events of the wifi
 */
static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    // If the event is a wifi event
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();// Connect to the access point

    } // If the event is a wifi event and the station is disconnected
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        // If the number of retries is less than the maximum number of retries
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();// Connect to the access point
            s_retry_num++; // Increase the number of retries
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);// Set the fail bit
        }
        ESP_LOGI(TAG,"connect to the AP fail");

    } // If the event is an IP event and the station got an IP 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;// Get the IP event
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip)); 
        s_retry_num = 0;// Set the number of retries to 0
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);// Set the connected bit
    }
    // If the event is an AP event and the station is connected
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        // Get the event
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);// Print the station that joined
    } // If the event is an AP event and the station is disconnected
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        // Get the event
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d, reason=%d",
                 MAC2STR(event->mac), event->aid, event->reason);
    }
}

/**
 * @brief Function to initialize the wifi as a station and as an access point
 * 
 * @return void
 * 
 * This function is used to initialize the wifi as a station and as an access point
 */

void wifi_init_apsta(void)
{
    s_wifi_event_group = xEventGroupCreate();// Create the event group

    ESP_ERROR_CHECK(esp_netif_init());// Initialize the network interface

    ESP_ERROR_CHECK(esp_event_loop_create_default()); // Create the default event loop
    esp_netif_create_default_wifi_sta(); // Create the default wifi station
    esp_netif_create_default_wifi_ap(); // Create the default wifi access point

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT(); // Initialize the wifi configuration
    ESP_ERROR_CHECK(esp_wifi_init(&cfg)); // Initialize the wifi

    // Instance of the event handler
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip; 

    // Register the event handler
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    // Register the event handler
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));
    // Register the event handler
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                    ESP_EVENT_ANY_ID,
                                                    &event_handler,
                                                    NULL,
                                                    NULL));

    /*         Initialized configuration station            */                                                 
    wifi_config_t wifi_config_est = {
        .sta = {
            .ssid = "JAVIER_RED", // Name of access point to connect
            .password = "12345678J", // Password of access point to connect
            /* Authmode threshold resets to WPA2 as default if password matches WPA2 standards (password len => 8).
             * If you want to connect the device to deprecated WEP/WPA networks, Please set the threshold value
             * to WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK and set the password with length and format matching to
             * WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK standards.
             */
            .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD, // Threshold of the authentication mode
            .sae_pwe_h2e = ESP_WIFI_SAE_MODE, // SAE mode
            .sae_h2e_identifier = EXAMPLE_H2E_IDENTIFIER, // H2E identifier
        },
    };

    /*         Initialized configuration access point            */
    wifi_config_t wifi_config_ap = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID, // Name of the access point
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID), // Length of the name of the access point
            .channel = EXAMPLE_ESP_WIFI_CHANNEL, // Channel of the access point
            .password = EXAMPLE_ESP_WIFI_PASS, // Password of the access point
            .max_connection = EXAMPLE_MAX_STA_CONN, // Maximum number of stations that can connect to the access point
#ifdef CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT // If the SAE support is defined
            .authmode = WIFI_AUTH_WPA3_PSK,// Authentication mode
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,// SAE mode
#else /* CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT */ // If the SAE support is not defined
            .authmode = WIFI_AUTH_WPA2_PSK, // Authentication mode
#endif
            // PMF configuration
            .pmf_cfg = {
                    .required = true,
            },
        },
    };
    // If the password of the access point is empty
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        // Set the authentication mode to open
        wifi_config_ap.ap.authmode = WIFI_AUTH_OPEN;
    }

    // Set the configuration of the station and the access point
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA) );
    // Set the configuration reds
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config_est));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config_ap));
    // Start the wifi
    ESP_ERROR_CHECK(esp_wifi_start() );


    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    // If the station is connected
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "The station connected to ap SSID:%s , password:%s",
                 "JAVIER_RED", "12345678J");
    }// If the station failed to connect 
    else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 "JAVIER_RED", "12345678J");
    }// If the event is unexpected 
    else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
    ESP_LOGI(TAG, "wifi_init_sta finished.");// Print that the initialization of the station is finished

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
            EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS, EXAMPLE_ESP_WIFI_CHANNEL);

}

void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_APSTA"); // Print the mode of the wifi
    wifi_init_apsta(); // Initialize the wifi as a station and as an access point
}
