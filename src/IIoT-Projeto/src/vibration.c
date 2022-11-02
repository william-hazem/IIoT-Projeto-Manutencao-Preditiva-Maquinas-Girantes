#include "vibration.h"

#include <mpu6050/mpu6050.h>
#include <memory.h>

#include "oled.h"

uint8_t vbr_init() {
    
    vbr_head = vbr_data;
    vbr_end = vbr_data + VBR_MAX_BUFFER_SIZE;
    memset(vbr_head, 0x0, VBR_MAX_BUFFER_SIZE);
    mpu6050_init();
    #ifdef TESTE
        uint16_t tmp[] = VBR_DEFAULT_DATA;
        memcpy(vbr_data, tmp, sizeof(uint16_t) * VBR_MAX_BUFFER_SIZE);
        ets_printf("dados carregados: "); 
        for(int i = 0; i < 5; i++)
            ets_printf("%d ", tmp[i]);
        ets_printf("\n");   
    #endif

    return 0;
}

void vbr_task(void *args) {
    bool first_run = true;
    for(;;)
    {
        if(vbr_head == vbr_end) // verifica se a referência estar no final da memória alocada
            vbr_head = vbr_data;

        #ifndef TESTE
        *vbr_head = mpu6050_get_acceleration_x();
        #else
            ets_printf("valor: %d", *vbr_head);
        #endif

        if(first_run)
        {
            vbr_min = *vbr_head;
            vbr_max = *vbr_head;
            first_run = false;
        }

        if(*vbr_head < vbr_min)
            vbr_min = *vbr_head;


        g_dados.vbr_mim = vbr_min;
        g_dados.vbr_max = vbr_max;

        vbr_head = vbr_head + 1;
        vTaskDelay(VBR_DELAY / portTICK_PERIOD_MS);
    }
}