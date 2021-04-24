#ifndef HTTP_H
#define HTTP_H

#include "esp_http_client.h"

/* HTTP event handler */
esp_err_t http_event_handler(esp_http_client_event_t *evt);

/* HTTP perform request */
bool http_perform_request(const char *url, char *buffer, const char *cert);

#endif