#include "vibration.h"

#include <mpu6050/mpu6050.h>
#include <memory.h>

#include "common.h" // g_dados

#include "i2c_bus_mng.h"
#include "task_priorities.h"

#define AC2G_SCALE 16384.0f //< escala para escala de +/- 2g

uint8_t vbr_init() {
    
    vbr_head = vbr_data;
    vbr_end = vbr_data + VBR_MAX_BUFFER_SIZE;
    memset(vbr_head, 0x0, sizeof(uint16_t) * VBR_MAX_BUFFER_SIZE);
    
    g_dados.vbr_begin = vbr_data;
    g_dados.vbr_end   = vbr_data + VBR_MAX_BUFFER_SIZE;
    g_dados.vbr_i = vbr_data; 
    g_dados.vbr_j = vbr_data; 

    uint8_t ret = 0;
    I2C_WAIT_UNTIL_FREE(
        mpu6050_init();
        ret = mpu6050_test_connection();
        // ets_printf("RET %u\n", ret)
    
        , 10);
    #ifdef TESTE
        uint16_t tmp[] = VBR_DEFAULT_DATA;
        memcpy(vbr_data, tmp, sizeof(uint16_t) * VBR_MAX_BUFFER_SIZE); 
    #endif

    return ret;
}
#include "esp_log.h"
void vbr_task(void *args) {
    bool first_run = true;


    float *vbr_min = &g_dados.vbr_mim;
    float *vbr_max = &g_dados.vbr_max;
    g_dados.vbr_count = 0;
    for(;;)
    {
        if(vbr_head == vbr_end) // verifica se a referência estar no final da memória alocada
            vbr_head = vbr_data;

        #ifndef TESTE
        // ets_printf("MAKING RUN\n");
        if(xSemaphoreTake(i2c_mutex, (TickType_t) 10) != pdTRUE)
            continue;
        
        *vbr_head = mpu6050_get_acceleration_x() / AC2G_SCALE;
        // ets_printf("READING: %d\n", (int)*vbr_head);
        g_dados.vbr_j = vbr_head;
        g_dados.vbr_count += 1;
        
        xSemaphoreGive(i2c_mutex);
        #else
            // ets_printf("valor: %d", *vbr_head);
        #endif

        if(first_run)
        {
            *vbr_min = *vbr_head;
            *vbr_max = *vbr_head;
            first_run = false;
        }

        if(*vbr_head < *vbr_min)
            *vbr_min = *vbr_head;
        if(*vbr_head > *vbr_max)
            *vbr_max = *vbr_head;


        vbr_head = vbr_head + 1;
        vTaskDelay(TASK_PERIOD(AQUISITION) / portTICK_PERIOD_MS);
    }
}