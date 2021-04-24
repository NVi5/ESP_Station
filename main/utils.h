#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

/* Delay seconds to ticks */
#define D_SEC(x) ( x * 1000 / portTICK_PERIOD_MS )
/* Delay minutes to ticks */
#define D_MIN(x) ( x * 1000 * 60 / portTICK_PERIOD_MS )

/*!
 *  @brief Function that creates a mandatory delay required in some of the APIs.
 *
 * @param[in] period              : Delay in microseconds.
 * @param[in, out] intf_ptr       : Void pointer that can enable the linking of descriptors
 *                                  for interface related call backs
 *  @return void.
 *
 */
void utils_delay_us(uint32_t period, void *intf_ptr);

#endif