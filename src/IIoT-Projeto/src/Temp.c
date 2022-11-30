#include "Temp.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "task_priorities.h"

void Temp_tesk(void *args) {
    int adcVal = 0;
    float milliVolt = 0.0;
    tempC = .0f;
    adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11); // ~ 2650mv
    
    for(;;)
    {
        adc2_get_raw(ADC1_CHANNEL_5, ADC_WIDTH_BIT_10, &adcVal);
        milliVolt = adcVal * (5000.0 / 1024.0);

        tempC = milliVolt / 10;

        vTaskDelay(TASK_PERIOD(TEMP) / portTICK_PERIOD_MS);
    }
}