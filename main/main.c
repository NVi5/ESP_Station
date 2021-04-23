#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_tls.h"
#include "esp_ota_ops.h"
#include "esp_https_ota.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_http_client.h"
#include "wifi_functions.h"

#define D_SEC(x) ( x * 1000 / portTICK_PERIOD_MS )
#define D_MIN(x) ( x * 1000 * 60 / portTICK_PERIOD_MS )

/* URL to latest build version */
#define HTTP_OTA_VERSION            "https://esp32ms.000webhostapp.com/uploads/latestBuildVersion.txt"
/* URL to OTA bin file */
#define HTTP_OTA_FILE               "https://esp32ms.000webhostapp.com/uploads/esp32.bin"
/* HTTP buffer size */
#define HTTP_BUFFER_SIZE            20
/* OTA period check in minutes */
#define OTA_CHECK_PERIOD            D_MIN(5)
/* OTA max length of build version string */
#define OTA_VERSION_MAX_LEN         5

static const char *TAG = "APP";

extern const char server_root_cert_pem_start[] asm("_binary_cert_pem_start");
extern const char server_root_cert_pem_end[]   asm("_binary_cert_pem_end");

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static int output_len;       // Stores number of bytes read
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            /*
             *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
             *  However, event handler can also be used in case chunked encoding is used.
             */
            if (!esp_http_client_is_chunked_response(evt->client))
            {
                // If user_data buffer is configured, copy the response into the buffer
                if (evt->user_data) {
                    memcpy(evt->user_data + output_len, evt->data, evt->data_len);
                }
                output_len += evt->data_len;
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            output_len = 0;
            int mbedtls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error(evt->data, &mbedtls_err, NULL);
            if (err != 0)
            {
                ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
                ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            }
            break;
    }
    return ESP_OK;
}

static void perform_ota_update(void)
{
    ESP_LOGI(TAG, "Starting OTA example");

    esp_http_client_config_t config = {
        .url = HTTP_OTA_FILE,
        .cert_pem = server_root_cert_pem_start,
        .event_handler = _http_event_handler,
    };

    esp_err_t ret = esp_https_ota(&config);
    if (ret == ESP_OK) {
        esp_restart();
    } else {
        ESP_LOGE(TAG, "Firmware upgrade failed");
    }
}

static bool get_remote_fw_version(char *buffer)
{
    esp_http_client_config_t config = {
        .url = HTTP_OTA_VERSION,
        .event_handler = _http_event_handler,
        .user_data = buffer,
        .cert_pem = server_root_cert_pem_start,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
        return false;
    }

    ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d", esp_http_client_get_status_code(client), esp_http_client_get_content_length(client));
    esp_http_client_cleanup(client);

    return true;
}

static bool is_new_fw_available(void)
{
    char local_response_buffer[HTTP_BUFFER_SIZE] = {0};

    ESP_LOGI(TAG, "Check for new firmware version");
    if (get_remote_fw_version(local_response_buffer) == false)
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

static void ota_task(void *pvParameters)
{
    while (1)
    {
        if (is_new_fw_available())
        {
            ESP_LOGI(TAG, "New version available, performing OTA");
            perform_ota_update();
        }

        vTaskDelay(OTA_CHECK_PERIOD);
    }
}

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

    wifi_initialise();
    wifi_wait_connected();
    ESP_LOGI(TAG, "Connected to wifi");

    xTaskCreate(&ota_task, "ota_task", 8192, NULL, 5, NULL);
}
