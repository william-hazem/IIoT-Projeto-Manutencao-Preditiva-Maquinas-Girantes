#include <ssd1306.h>
#include <keypad.h>
#include <string.h>

#include <mpu6050/mpu6050.h>
#include <esp_err.h> 
#include <freertos/FreeRTOS.h> 
#include <freertos/task.h>

#include "i2c_bus_mng.h"
#include "oled.h"
#include "task_priorities.h"
#include "vibration.h"



void app_main() 
{

    /// Inicializa o driver i2c
    i2c_bus_init();

    gpio_num_t RST_BUTTON = GPIO_NUM_34;
    gpio_set_direction(RST_BUTTON, GPIO_MODE_INPUT);

    init_oled();
    vbr_init();

    xTaskCreate(vbr_task, "AQUISITION TASK", 8000, NULL, AQUISITION_PRIORITY, NULL);
    xTaskCreate(display_task, "DISPLAY TASK", 8000, NULL, DISPLAY_DATA, NULL);
    

    if(mpu6050_test_connection())
        ets_printf("RESPONDING\n");
    
    
    for(;;)
    {
        
        if(!gpio_get_level(RST_BUTTON)) /// apenas simula o reset das informações
        {
            while(!gpio_get_level(RST_BUTTON));
            display_reset();
            continue;
        }
        
        
        vTaskDelay(66 / portTICK_PERIOD_MS);
    }
}


