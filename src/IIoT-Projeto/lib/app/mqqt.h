#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "common.h"
#include "azure-key.h"

#define W2P "devices/"IOTHUB_DEVID"/messages/events/"

static const char *TAGM = "MQTT_EXAMPLE";

bool mqttConnected;

esp_mqtt_client_handle_t client;


static void log_error_if_nonzero(const char * message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAGM, "Last error %s: 0x%x", message, error_code);
    }
}

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            mqttConnected = true;
            ESP_LOGI(TAGM, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_publish(client, W2P, IOTHUB_DEVID" connected", 0, 1, 1);
            ESP_LOGI(TAGM, "sent publish successful, msg_id=%d", msg_id);

            break;
        case MQTT_EVENT_DISCONNECTED:
            mqttConnected = false;
            ESP_LOGI(TAGM, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            // ESP_LOGI(TAGM, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            // msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
            // ESP_LOGI(TAGM, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAGM, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAGM, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAGM, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAGM, "MQTT_EVENT_ERROR");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
                log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
                log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
                ESP_LOGI(TAGM, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

            }
            break;
        default:
            ESP_LOGI(TAGM, "Other event id:%d", event->event_id);
            
            
            break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAGM, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb((esp_mqtt_event_handle_t) event_data);
}

static void mqtt_app_start(void)
{
    mqttConnected = false;
    ESP_LOGD(TAGM, "inicializando cliente mqtt");
    const char* username = IOTHUB_NAME".azure-devices.net/"IOTHUB_DEVID"/?api-version=2021-04-12";
    ESP_LOGD(TAGM, "mqtt username: %s", username);
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtts://"IOTHUB_NAME".azure-devices.net",
        .port=8883,
        .username = username,
        .client_id = IOTHUB_DEVID,
        .password = IOTHUB_KEY,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}

/**
 * @brief Publica os dados da aplicação no Broker
 */
char buffer_vbr[3000];
void mqtt_publicar(void)
{
    if(!mqttConnected)
        return;

    char buffer[32] = "";
    
    sprintf(buffer, "%.2f", g_dados.temperatura);
    esp_mqtt_client_publish(client, W2P"temp", buffer, strlen(buffer), 1, 1);
    sprintf(buffer, "%.2f", g_dados.vbr_rms);
    esp_mqtt_client_publish(client, W2P"/accRMS", buffer, strlen(buffer), 1, 1);
    
    // esp_mqtt_client_publish(client, W2P"/rms", buffer, strlen(buffer), 1, 1);

    // memcpy(buffer_vbr, g_dados.vbr_begin, g_dados.vbr_end - g_dados.vbr_begin);
    // esp_mqtt_client_publish(client, W2P"/vbr_raw", buffer_vbr, 3000, 1, 1);
}