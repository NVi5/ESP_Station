#include "utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*!
 * @brief This function provides the delay for required time (Microseconds) as per the input provided in some of the
 * APIs
 */
void utils_delay_us(uint32_t period, void *intf_ptr)
{   uint32_t ticks = (period / 1000) / portTICK_PERIOD_MS;
    if (ticks == 0) ticks = 1;
    vTaskDelay(ticks);
}