#include "oled.h"
#include "datalogger.h"
#include <esp_log.h>

#include <rms.h>
#include <mpu6050/mpu6050.h>

#include "i2c_bus_mng.h"
#include "task_priorities.h"
#include "Temp.h"

#include "common.h"     // app_t


char* alignCenter(char* text) {
    char tmp[17];
    memset(tmp, 0x0, sizeof(tmp));
    int size = strlen(text);
    int offset = (17 - size) / 2;
    memcpy(tmp + offset, text, strlen(text));
    memcpy(text, tmp, sizeof(tmp));
    return text;
}

void init_oled() {
    /// inicializa a interface i2c
    memset(g_dados.hora, 0x0, sizeof(char) * 32);
    g_dados.vbr_begin = NULL;
    g_dados.vbr_end = NULL;

    i2c_master_init(&g_oled, I2C_SDA, I2C_SDL, -1);

    ssd1306_init(&g_oled, 128, 64);
}

void startup_screen()
{
    ssd1306_clear_screen(&g_oled, false);
    
    char *text = malloc(sizeof(char)*17);
    strcpy(text, "IOT");
    ssd1306_display_text(&g_oled, 1, alignCenter(text), 17, false);
    strcpy(text, "INDUSTRIAL");
    ssd1306_display_text(&g_oled, 2, alignCenter(text), 22, false);
    strcpy(text, "2022");
    ssd1306_display_text(&g_oled, 3, alignCenter(text), 22, false);

    strcpy(text, "HELLO WORLD");
    ssd1306_display_text(&g_oled, 5, alignCenter(text), 22, false);
}

void display_reset()
{
    g_dados.vbr_max = 0;
    g_dados.vbr_mim = 0;
    g_dados.vbr_rms = 0;
}

void display_task(void *args)
{
    // while(xSemaphoreTake(i2c_mutex, ( TickType_t ) 10 ) != pdTRUE) {}
    // startup_screen();
    // vTaskDelay(500 / portTICK_PERIOD_MS);
    // xSemaphoreGive(i2c_mutex);

    char buffer1[22] = "";
    char buffer2[22] = "";
    char buffer3[22] = "";
    char buffer4[22] = "";
    char strtemp[22] = "";
    
    I2C_WAIT_UNTIL_FREE(ssd1306_clear_screen(&g_oled, false), 5)

    float * vbr_ref = g_dados.vbr_begin;
    for(;;) if(xSemaphoreTake(i2c_mutex, ( TickType_t ) 5 ) == pdTRUE)
    {

        int n = vbr_ref- g_dados.vbr_begin;
        g_dados.vbr_rms = rmsf(g_dados.vbr_begin, n);
        vbr_ref = g_dados.vbr_j;

        float temp = mpu6050_get_temperature()/340 + 36.53;
        g_dados.temperatura = temp;
        
        sprintf(buffer1, "max: %.2f", g_dados.vbr_max);
        sprintf(buffer2, "min: %.2f", g_dados.vbr_mim);
        sprintf(buffer3, "rms: %.2f", g_dados.vbr_rms);


        sprintf(strtemp," %.2fC", g_dados.temperatura);
        strcat(buffer4, g_dados.hora);
        strcat(buffer4, strtemp);

        ssd1306_clear_line(&g_oled, 1, false);
        ssd1306_display_text(&g_oled, 1, buffer4, 22, false);

        ssd1306_clear_line(&g_oled, 3, false);
        ssd1306_display_text(&g_oled, 3, buffer1, 22, false);

        ssd1306_clear_line(&g_oled, 5, false);
        ssd1306_display_text(&g_oled, 5, buffer2, 22, false);

        ssd1306_clear_line(&g_oled, 7, false);
        ssd1306_display_text(&g_oled, 7, buffer3, 22, false);

        xSemaphoreGive(i2c_mutex);

        memset(buffer1, 0x0, 22);
        memset(buffer2, 0x0, 22);
        memset(buffer3, 0x0, 22);
        memset(buffer4, 0x0, 22);
        vTaskDelay(TASK_PERIOD(DISPLAY) / portTICK_PERIOD_MS);
    }
}

void update_hora_task(void *args)
{
    int cont = 0;
    while (xSemaphoreTake(i2c_mutex, ( TickType_t ) 10 ) != pdTRUE) {vTaskDelay(5);}
    
    struct tm set = {
        .tm_year = 2022,
        .tm_mon  = 10,  // 0-based
        .tm_mday = 07,
        .tm_hour = 6,
        .tm_min  = 58,
        .tm_sec  = 0
    }; 
    // ds1307_set_time(&dev, &set);
    
    struct tm data;

    ds1307_get_time(&dev, &data);
    ESP_LOGI(pcTaskGetTaskName(0), "%04d-%02d-%02d %02d:%02d:%02d",
                data.tm_year, data.tm_mon + 1,
                data.tm_mday, data.tm_hour, data.tm_min, data.tm_sec);
    vTaskDelay(10);
    
    xSemaphoreGive(i2c_mutex);
    // ets_printf("%02d:%02d:%02d", data.tm_hour, data.tm_min, data.tm_sec);

    time_t last = time(NULL);
    char buffer[32] = "vazio";
    while(true) 
    {  
        data.tm_sec += time(NULL) - last;
        if(data.tm_sec >= 60)
        {
            data.tm_sec -= 60;
            data.tm_min += 1;
        }
        if(data.tm_min >= 60)
        {
            data.tm_min -= 60;
            data.tm_hour += 1;
        }
        if(data.tm_hour >= 24)
        {
            data.tm_hour -= 24;
        }
        
        sprintf(buffer, "%02d:%02d:%02d ", data.tm_hour, data.tm_min, data.tm_sec);
        strcpy(g_dados.hora, buffer);
        
        
        last = time(NULL);
        vTaskDelay(TASK_PERIOD(DISPLAY) / portTICK_PERIOD_MS);
    }
    
}