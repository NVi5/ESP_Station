#ifndef OTA_H
#define OTA_H

#include "utils.h"

/* URL to latest build version */
#define HTTP_OTA_VERSION            "https://esp32ms.000webhostapp.com/uploads/latestBuildVersion.txt"
/* URL to OTA bin file */
#define HTTP_OTA_FILE               "https://esp32ms.000webhostapp.com/uploads/esp32.bin"
/* OTA period check in minutes */
#define OTA_CHECK_PERIOD            D_MIN(60)
/* OTA max length of build version string */
#define OTA_VERSION_MAX_LEN         5
/* FW VERSION buffer size */
#define FW_VER_BUFFER_SIZE          20

/* Task responsible for OTA update */
void ota_task(void *pvParameters);

#endif