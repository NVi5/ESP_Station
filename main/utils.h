#ifndef UTILS_H
#define UTILS_H

/* Delay seconds to ticks */
#define D_SEC(x) ( x * 1000 / portTICK_PERIOD_MS )
/* Delay minutes to ticks */
#define D_MIN(x) ( x * 1000 * 60 / portTICK_PERIOD_MS )

#endif