#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "ota.h"
#include "wifi.h"

static const char *TAG = "MAIN";

static void app_init(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
}

void app_main(void)
{
    ESP_LOGI(TAG, "SSID: "SSID);
    ESP_LOGI(TAG, "Build Version: %d", BUILD_VERSION);

    app_init();
    wifi_init();

    xTaskCreate(&ota_task, "ota_task", 8192, NULL, 5, NULL);
}
