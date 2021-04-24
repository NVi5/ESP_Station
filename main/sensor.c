#include "sensor.h"
#include "esp_log.h"
#include "user_i2c.h"
#include "utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bme280_defs.h"
#include "bme280.h"
#include "http.h"

static const char *TAG = "SENSOR";
static const char *JSON = "{\"temperature\": %.2f, \"humidity\": %.2f, \"pressure\": %.2f}";
static struct bme280_dev dev = {0};
static uint8_t dev_addr = BME280_I2C_ADDR_PRIM;
static uint32_t req_delay;

static bool sensor_init(void)
{
    ESP_LOGI(TAG, "sensor_init start");
    int8_t rslt = BME280_OK;
    uint8_t settings_sel;

    dev.intf_ptr = &dev_addr;
    dev.intf = BME280_I2C_INTF;
    dev.read = user_i2c_read;
    dev.write = user_i2c_write;
    dev.delay_us = utils_delay_us;

    rslt = bme280_init(&dev);
    if (rslt != BME280_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize the device (code %+d).\n", rslt);
        return false;
    }

    dev.settings.osr_h = BME280_OVERSAMPLING_16X;
    dev.settings.osr_p = BME280_OVERSAMPLING_16X;
    dev.settings.osr_t = BME280_OVERSAMPLING_16X;
    dev.settings.filter = BME280_FILTER_COEFF_OFF;

    settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;

    rslt = bme280_set_sensor_settings(settings_sel, &dev);
    req_delay = bme280_cal_meas_delay(&dev.settings);

    if (rslt != BME280_OK)
    {
        ESP_LOGE(TAG, "Failed to set sensor settings (code %+d).", rslt);
        return false;
    }

    ESP_LOGI(TAG, "sensor_init succeeded");
    return true;
}

static void sensor_print_data(struct bme280_data *comp_data)
{
    printf("Temp: %0.2f, Pressure: %0.2f, Humidity: %0.2f\r\n",comp_data->temperature, comp_data->pressure, comp_data->humidity);
}

static void sensor_get_json_from_data(struct bme280_data *comp_data, char *buffer)
{
    sprintf(buffer, JSON, comp_data->temperature, comp_data->humidity, comp_data->pressure);
    ESP_LOGI(TAG, "JSON: %s", buffer);
}

static bool sensor_perform_measurement(struct bme280_data *comp_data)
{
    int8_t rslt = BME280_OK;
    ESP_LOGI(TAG, "Set BME280_FORCED_MODE");
    rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, &dev);
    if (rslt != BME280_OK)
    {
        ESP_LOGE(TAG, "Failed to set sensor mode (code %+d).", rslt);
        return false;
    }

    vTaskDelay(req_delay / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "Get BME280_ALL");
    rslt = bme280_get_sensor_data(BME280_ALL, comp_data, &dev);
    if (rslt != BME280_OK)
    {
        ESP_LOGE(TAG, "Failed to get sensor data (code %+d).", rslt);
        return false;
    }

    comp_data->pressure /= 100;
    return true;
}

static bool sensor_verify_measurement(struct bme280_data *comp_data)
{
    static float last_press = 0;
    static uint8_t number_of_retries = 0;

    if (comp_data->pressure < 870.0f)
    {
        ESP_LOGW(TAG, "sensor_verify_measurement pressure too low");
        number_of_retries++;
        return false;
    }
    if (last_press < 1.0f)
    {
        ESP_LOGI(TAG, "sensor_verify_measurement last_press not set, return true");
        last_press = comp_data->pressure;
        number_of_retries = 0;
        return true;
    }
    if ((last_press - comp_data->pressure) > 0.5f || (last_press - comp_data->pressure) < -0.5f)
    {
        ESP_LOGW(TAG, "last_press=%f, new_press=%f",last_press, comp_data->pressure);
        if (number_of_retries > MAX_MEAS_RETRIES)
        {
            ESP_LOGW(TAG, "sensor_verify_measurement maximum number of retries reached, setting new last_press");
            last_press = comp_data->pressure;
            number_of_retries = 0;
            return true;
        }
        else
        {
            ESP_LOGW(TAG, "sensor_verify_measurement pressure difference too big, retrying");
            number_of_retries++;
            return false;
        }
    }

    last_press = comp_data->pressure;
    number_of_retries = 0;
    return true;
}

static bool sensor_handle_measurement(struct bme280_data *comp_data)
{
    if (sensor_perform_measurement(comp_data) == false)
    {
        return false;
    }

    sensor_print_data(comp_data);

    if (sensor_verify_measurement(comp_data) == false)
    {
        return false;
    }

    return true;
}

void sensor_task(void *pvParameters)
{
    struct bme280_data comp_data;
    char json[JSON_BUFFER_SIZE] = {0};

    if (sensor_init() == false)
    {
        ESP_LOGE(TAG, "sensor_init returned false, returning from task");
        return;
    }

    while(1)
    {
        if (sensor_handle_measurement(&comp_data))
        {
            ESP_LOGI(TAG, "Measurement succeeded, waiting %d ticks", SENSOR_READ_PERIOD);
            sensor_get_json_from_data(&comp_data, json);
            http_post_thinger_io(json);
            vTaskDelay(SENSOR_READ_PERIOD);
        }
        else
        {
            ESP_LOGW(TAG, "Measurement failed, retrying after %d ticks", SENSOR_RETRY_PERIOD);
            vTaskDelay(SENSOR_RETRY_PERIOD);
        }
    }
}