idf_component_register(SRCS "EspNowDriver.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver
                    REQUIRES esp_wifi
                    PRIV_REQUIRES nvs_flash)
