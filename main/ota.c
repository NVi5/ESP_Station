#include "ota.h"
#include <string.h>
#include "esp_https_ota.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "http.h"

static const char *TAG = "OTA";

extern const char server_root_cert_pem_start[] asm("_binary_cert_pem_start");
extern const char server_root_cert_pem_end[]   asm("_binary_cert_pem_end");

static void ota_perform_ota_update(void)
{
    ESP_LOGI(TAG, "Starting OTA example");

    esp_http_client_config_t config = {
        .url = HTTP_OTA_FILE,
        .cert_pem = server_root_cert_pem_start,
        .event_handler = http_event_handler,
    };

    esp_err_t ret = esp_https_ota(&config);
    if (ret == ESP_OK) {
        esp_restart();
    } else {
        ESP_LOGE(TAG, "Firmware upgrade failed");
    }
}

static inline bool ota_get_remote_fw_version(char *buffer)
{
    return http_perform_request(HTTP_OTA_VERSION, buffer, server_root_cert_pem_start);
}

static bool ota_is_new_fw_available(void)
{
    char local_response_buffer[FW_VER_BUFFER_SIZE] = {0};

    ESP_LOGI(TAG, "Check for new firmware version");
    if (ota_get_remote_fw_version(local_response_buffer) == false)
    {
        ESP_LOGW(TAG, "Could not get firmware version");
        return false;
    }

    ESP_LOGI(TAG, "%s", local_response_buffer);

    if (strlen(local_response_buffer) > OTA_VERSION_MAX_LEN)
    {
        ESP_LOGW(TAG, "Received response is too long");
        return false;
    }

    char * endptr;
    uint16_t remote_version = (uint16_t)strtol(local_response_buffer, &endptr, 10);
    if (endptr <= local_response_buffer)
    {
        ESP_LOGW(TAG, "Could not parse response");
        return false;
    }

    ESP_LOGI(TAG, "Remote Build Version: %d", remote_version);
    if (remote_version > BUILD_VERSION)
    {
        ESP_LOGI(TAG, "Remote has newer version");
        return true;
    }
    else
    {
        ESP_LOGI(TAG, "Remote version is same or lower than local");
        return false;
    }
}

void ota_task(void *pvParameters)
{
    while (1)
    {
        if (ota_is_new_fw_available())
        {
            ESP_LOGI(TAG, "New version available, performing OTA");
            ota_perform_ota_update();
        }

        vTaskDelay(OTA_CHECK_PERIOD);
    }
}