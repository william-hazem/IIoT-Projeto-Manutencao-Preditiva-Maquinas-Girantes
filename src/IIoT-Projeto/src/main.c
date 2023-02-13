#include <ssd1306.h> 
#include <string.h>

#include <esp_sleep.h>
#include <esp_err.h> 
#include <freertos/FreeRTOS.h> 
#include <freertos/task.h>
#include <time.h>

#include "i2c_bus_mng.h"
#include "oled.h"
#include "task_priorities.h"
#include "vibration.h"
#include "datalogger.h"
#include "Temp.h"

#include <wifi.h>
#include <mqtt.h>
#include <http.h>
#include <nvs_flash.h>

#define TAGM "APP"

#define MAIN 1

#if MAIN

void show_heap(void * args)
{
    for(;;)
    {
        ESP_LOGI("wifi", "free Heap:%d", esp_get_free_heap_size());
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}


QueueHandle_t button_queue = NULL;
void button_mng_task()
{
    gpio_num_t RST_BUTTON = GPIO_NUM_34;
    gpio_set_direction(RST_BUTTON, GPIO_MODE_INPUT);
    static gpio_isr_handle_t handle = NULL;
    // gpio_isr_register();
    // gpio_isr_handler_add(RST_BUTTON, handle, (void*)0);
    bool wasPressed = false;
    time_t lastTime = time(NULL);
    time_t pressTime = time(NULL);
    xQueueCreate(5, sizeof(int)*8);
    for(;;)
    {
        
        while(gpio_get_level(RST_BUTTON))   // tempo de pressão sobre o botão
        {
            pressTime = time(NULL) - lastTime;
            lastTime = time(NULL);
            wasPressed = true;   
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
        
        if(wasPressed && pressTime <= 1000) // reset display
        {
            display_reset();
        } else
        if(wasPressed && pressTime <= 3000) // desmonta sistema de arquivos
        {
            /// do something
        }

        wasPressed = false;
        pressTime = 0;
        
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void app_main() 
{
    esp_err_t ret_flash = nvs_flash_init();
    if (ret_flash == ESP_ERR_NVS_NO_FREE_PAGES || ret_flash == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret_flash = nvs_flash_init();
        ESP_LOGE("main", "-> NVS ERASED");
    }

    // xTaskCreate(show_heap, "CPU STATS", 2048, NULL, 6, NULL);
    // wifi_connection_t conn = {.ssid = "brisa-2816643", .password="9oqgl1e0"};
    // wifi_connection_t conn = {.ssid = "TP-Link", .password="kwe12345"};
    // wifi_register_connection(conn);
    if(wifi_init_sta() == ESP_OK)
        mqtt_app_start();
    // http_init_server();
    
    i2c_bus_init();
    

    // ets_printf("-- TASK DIVISION --\n");
    // ets_printf("DISPLAY PERIOD     \t%d\t%d\n", TASK_PERIOD(DISPLAY));
    // ets_printf("ACQUISITION PERIOD \t%u\n",     TASK_PERIOD(AQUISITION));
    // ets_printf("-------------------\n");
    
    datalogger_init();
    init_oled();
    uint8_t ret = vbr_init();

    if(ret)
        ESP_LOGI(TAGM, "VBR initialized\n");
    else
        ESP_LOGE(TAGM, "VBR NOT initialized\n");
      
    

    // ets_printf("INIT FINISHED\n");
    // if(setTime() == ESP_OK)
    //     ets_printf("Tempo definido\n");
    // else
    //     ets_printf("ERRO RTC\n");

    xTaskCreate(display_task, "DISPLAY TASK", 2048, NULL, DISPLAY_DATA, &gth_display);
    xTaskCreate(update_hora_task, "UPDATE HORA", 3000, NULL, DISPLAY_DATA, &gth_update_hora);
    xTaskCreate(vbr_task, "AQUISITION TASK", 2048, NULL, AQUISITION_PRIORITY, &gth_aquisition);
    xTaskCreate(Temp_tesk, "TEMPERATUDA TASK", 1024, NULL, TEMP_PRIORITY, &gth_temp);

    // ets_printf("SETUP FINISHED\n");

    time_t time_before = time(NULL);
    float* ref = g_dados.vbr_begin;
    for(;;)
    {

        if(g_dados.vbr_count >= 3000)
        {
            // vTaskSuspend(gth_aquisition);
            
            // ESP_LOGI(TAGM, "- %d reads on %ld", g_dados.vbr_count, time(NULL) - time_before);
            // #ifdef STATUS
            //     int n = vbr_head - ref;
            //     ref = vbr_head;
            //     n = n < 0 ? 3000 - n : n;

            // #endif
            
            // g_dados.vbr_count = 0;

            // vTaskSuspend(gth_display);
            // vTaskSuspend(gth_update_hora);

            
            mqtt_publicar();
            // esp_sleep_enable_timer_wakeup(WAKEUP_PERIOD);
            // esp_light_sleep_start();
            ESP_LOGI(TAGM, "$SLEEP");
            vTaskDelay(10000 / portTICK_PERIOD_MS);
            ESP_LOGI(TAGM, "$WAKEUP");

            /// re-inicializa as tasks após o sono profundo
            // vTaskResume(gth_display);
            // vTaskResume(gth_aquisition);
            xTaskCreate(vbr_task, "AQUISITION TASK", 2048, NULL, AQUISITION_PRIORITY, &gth_aquisition);
            // vTaskResume(gth_update_hora);

            time_before = time(NULL);
        }

        vTaskDelay(66 / portTICK_PERIOD_MS);

    }

    
}

#else

#endif
