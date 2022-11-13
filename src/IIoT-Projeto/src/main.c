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

const char* TAG = "APP";

void app_main() 
{

    /// Inicializa o driver i2c
    i2c_bus_init();
    

    gpio_num_t RST_BUTTON = GPIO_NUM_34;
    gpio_set_direction(RST_BUTTON, GPIO_MODE_INPUT);

    ets_printf("-- TASK DIVISION --\n");
    ets_printf("DISPLAY PERIOD %d\n", TASK_PERIOD(DISPLAY));
    ets_printf("DISPLAY PERIOD %u\n", TASK_PERIOD(AQUISITION));
    


    datalogger_init();
    init_oled();
    uint8_t ret = vbr_init();

    if(ret)
        ESP_LOGI(TAG, "VBR initialized\n");
    else
        ESP_LOGE(TAG, "VBR NOT initialized\n");
    
    

    ets_printf("INIT FINISHED\n");
    // if(setTime() == ESP_OK)
    //     ets_printf("Tempo definido\n");
    // else
    //     ets_printf("ERRO RTC\n");

    xTaskCreate(display_task, "DISPLAY TASK", 8000, NULL, DISPLAY_DATA, &gth_display);
    xTaskCreate(update_hora_task, "UPDATE HORA", 3000, NULL, DISPLAY_DATA, &gth_update_hora);
    xTaskCreate(vbr_task, "AQUISITION TASK", 8000, NULL, AQUISITION_PRIORITY, &gth_aquisition);
    xTaskCreate(Temp_tesk, "TEMPERATUDA TASK", 3000, NULL, TEMP_PRIORITY, &gth_temp);

    ets_printf("SETUP FINISHED\n");

    time_t time_before = time(NULL);
    for(;;)
    {

        if ((time(NULL) - time_before) >= 10)
        {
            
            // vTaskSuspend(gth_display);
            vTaskSuspend(gth_aquisition);
            // vTaskSuspend(gth_update_hora);

            

            esp_sleep_enable_timer_wakeup(WAKEUP_PERIOD);
            esp_light_sleep_start();

            /// re-inicializa as tasks após o sono profundo
            // vTaskResume(gth_display);
            vTaskResume(gth_aquisition);
            // vTaskResume(gth_update_hora);

            time_before = time(NULL);
        }

        if(!gpio_get_level(RST_BUTTON)) /// apenas simula o reset das informações
        {
            display_reset();
            // while(!gpio_get_level(RST_BUTTON)) vTaskDelay(1);
            
        }

        vTaskDelay(66 / portTICK_PERIOD_MS);

    }

    
}


