#include "http.h"

#include <esp_http_server.h>
#include <esp_log.h>

#include "base/base64.h"

extern const uint8_t f_index_start[] asm("_binary_index_html_start"); 
extern const uint8_t f_index_end[] asm("_binary_index_html_end"); 
extern const uint8_t f_script_start[] asm("_binary_script_js_start"); 
extern const uint8_t f_script_end[] asm("_binary_script_js_end");


static httpd_handle_t s_http_server_handle = NULL;

static const char* tag = "http sever";

static esp_err_t http_server_index_html_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)f_index_start, f_index_end - f_index_start);

    return ESP_OK;
}

static esp_err_t http_server_script_js_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_send(req, (const char *)f_script_start, f_script_end - f_script_start);

    return ESP_OK;
}

uint8_t http_init_server()
{
    httpd_config_t server_config = HTTPD_DEFAULT_CONFIG();
    /// aloca mais memória para o servidor http
    server_config.stack_size = 1024*8;
    esp_err_t err = httpd_start(&s_http_server_handle, &server_config);
    
    if(err != ESP_OK)
    {
        if(err == ESP_ERR_HTTPD_ALLOC_MEM)
            ESP_LOGE(tag, "Não foi possível alocar memória");
        else
            ESP_LOGE(tag, "Erro %s", esp_err_to_name(err));
        return ESP_FAIL;
    }

    httpd_uri_t index_html = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = http_server_index_html_handler,
            .user_ctx = NULL};
    httpd_register_uri_handler(s_http_server_handle, &index_html);

    httpd_uri_t script_js = {
            .uri = "/script.js",
            .method = HTTP_GET,
            .handler = http_server_script_js_handler,
            .user_ctx = NULL};
    httpd_register_uri_handler(s_http_server_handle, &script_js);

    // httpd_uri_t process = {
    //         .uri = "/process",
    //         .method = HTTP_GET,
    //         .handler = http_server_process_image,
    //         .user_ctx = NULL};
    // httpd_register_uri_handler(s_http_server_handle, &process);

    return 0;
}

uint8_t http_register_uri(void (*handler)())
{
    return 0;
}