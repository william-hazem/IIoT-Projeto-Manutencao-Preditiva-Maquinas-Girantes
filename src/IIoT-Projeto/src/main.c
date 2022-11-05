#include <ssd1306.h>
#include <keypad.h>
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

extern void app_main() 
{

    /// Inicializa o driver i2c
    i2c_bus_init();

    gpio_num_t RST_BUTTON = GPIO_NUM_34;
    gpio_set_direction(RST_BUTTON, GPIO_MODE_INPUT);

    init_oled();
    vbr_init();

    xTaskCreate(vbr_task, "AQUISITION TASK", 8000, NULL, AQUISITION_PRIORITY, &gth_aquisition);
    xTaskCreate(display_task, "DISPLAY TASK", 8000, NULL, DISPLAY_DATA, &gth_display);
    
    time_t time_before = time(NULL);
    for(;;)
    {
        
        if ((time(NULL) - time_before) >= 10)
        {

            vTaskSuspend(gth_display);
            vTaskSuspend(gth_aquisition);
            
            esp_sleep_enable_timer_wakeup(WAKEUP_PERIOD);
            esp_light_sleep_start();

            /// re-inicializa as tasks após o sono profundo
            vTaskResume(gth_display);
            vTaskResume(gth_aquisition);
            
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


