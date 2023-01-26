#include "Temp.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "task_priorities.h"
#include "common.h"             // g_dados

void Temp_tesk(void *args) {
    int adcVal = 0;
    float milliVolt = 0.0;
    tempC = .0f;
    esp_err_t ret = adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11); // ~ 2650mv
    adc1_config_width(ADC_WIDTH_10Bit);
    if(ret != ESP_OK) vTaskDelete(NULL);
    for(;;)
    {
        adcVal = adc1_get_raw(ADC1_CHANNEL_5);
        milliVolt = adcVal * (5000.0 / 1024.0);

        g_dados.temperatura = milliVolt / 10;

        vTaskDelay(TASK_PERIOD(TEMP) / portTICK_PERIOD_MS);
    }
}