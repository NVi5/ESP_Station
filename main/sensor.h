#ifndef SENSOR_H
#define SENSOR_H

/* Maximum number of measurement retries */
#define MAX_MEAS_RETRIES               5
/* SENSOR period check in minutes */
#define SENSOR_RETRY_PERIOD            D_SEC(3)
/* SENSOR period read in minutes */
#define SENSOR_READ_PERIOD             D_MIN(1)

/* Task responsible for handling sensor update */
void sensor_task(void *pvParameters);

#endif