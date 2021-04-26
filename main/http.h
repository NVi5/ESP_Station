#ifndef HTTP_H
#define HTTP_H

#include "esp_http_client.h"

/* HTTP event handler */
esp_err_t http_event_handler(esp_http_client_event_t *evt);

/* HTTP perform GET request */
bool http_get(const char *url, char *buffer, const char *cert);
/* HTTP perform POST request to Thinger.io */
bool http_post_thinger_io(char *data_json);
/* HTTP perform GET request with query to ThingSpeak */
bool http_post_thingspeak(char *url_data);

#endif