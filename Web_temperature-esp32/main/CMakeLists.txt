idf_component_register(SRCS "main.c" "Librerias/Wifi_lib/wifi_app.c" "Librerias/Adc_lib/ADC_NTC_POT.c" "Librerias/Uart_lib/COMANDS_UART.c" "Librerias/server_lib/http_server.c" "Librerias/rgb_lib/rgb_led.c"
                    INCLUDE_DIRS "."
                    EMBED_FILES webpage/app.css webpage/app.js webpage/favicon.ico webpage/index.html webpage/jquery-3.3.1.min.js )
