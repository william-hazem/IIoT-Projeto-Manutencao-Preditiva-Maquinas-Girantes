
#ifndef HTTP_H
#define HTTP_H

#include <inttypes.h>
#include <esp_http_client.h>

#ifdef __cplusplus
extern "C" {
#endif



uint8_t http_init_server();
uint8_t http_register_uri(void (*)(void));

#ifdef __cplusplus
}
#endif

#endif //!HTTP_H