#include "vibration.h"

#include <mpu6050/mpu6050.h>
#include <memory.h>
#include <time.h>

#include "common.h" // g_dados

#include "i2c_bus_mng.h"
#include "task_priorities.h"



#define AC2G_SCALE 16384.0f //< escala para escala de +/- 2g

float *vbr_min;
float *vbr_max;

uint8_t vbr_init() {
    
    vbr_head = vbr_data;
    vbr_end = vbr_data + VBR_MAX_BUFFER_SIZE;
    memset(vbr_head, 0x0, sizeof(uint16_t) * VBR_MAX_BUFFER_SIZE);
    
    /// setting global pointers to local addresses
    g_dados.vbr_begin = vbr_data;
    g_dados.vbr_end   = vbr_data + VBR_MAX_BUFFER_SIZE;
    g_dados.vbr_i = vbr_data; 
    g_dados.vbr_j = vbr_data; 
    g_dados.vbr_count = 0;

    vbr_min = &g_dados.vbr_mim;
    vbr_max = &g_dados.vbr_max;
    
    *vbr_min = 2;
    *vbr_max =  -2;

    uint8_t ret = 0;
    I2C_WAIT_UNTIL_FREE(
        mpu6050_init();
        ret = mpu6050_test_connection();
        // ets_printf("RET %u\n", ret)
    
        , 10);

    return ret;
}
#include "esp_log.h"
void vbr_task(void *args) {
    time_t elapsed = time(NULL);
    for(int i = 0; i < VBR_MAX_BUFFER_SIZE; i++)
    {  
        
        // *vbr_head = (float) g_dados.vbr_count;
        while(xSemaphoreTake(i2c_mutex, (TickType_t) 10) != pdTRUE)
            vTaskDelay(5 / portTICK_PERIOD_MS);
        
        vbr_head = vbr_data + i;
        
        *vbr_head = mpu6050_get_acceleration_x() / AC2G_SCALE;
        vbr_datay[i] = mpu6050_get_acceleration_y() / AC2G_SCALE;
        vbr_dataz[i] = mpu6050_get_acceleration_z() / AC2G_SCALE;
        
        xSemaphoreGive(i2c_mutex);
        
        g_dados.vbr_count = i + 1;

        g_dados.vbr_j = vbr_head;

        if(g_dados.vbr_count % 500 == 0) ESP_LOGI("VBR","counter %d", g_dados.vbr_count);
        
        /// get min/max from current read data
        *vbr_min = *vbr_head < *vbr_min ? *vbr_head : *vbr_min;
        *vbr_max = *vbr_head > *vbr_max ? *vbr_head : *vbr_max;

        vTaskDelay(TASK_PERIOD(AQUISITION) / portTICK_PERIOD_MS);
    }
    ESP_LOGI("VBR","FULL BUFFER (%ld ms)", (time(NULL) - elapsed)*1000l);
    vTaskDelete(NULL);

}