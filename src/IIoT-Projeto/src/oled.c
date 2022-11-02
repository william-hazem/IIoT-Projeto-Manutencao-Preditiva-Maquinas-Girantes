#include "oled.h"

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
    i2c_master_init(&g_oled, 23, 22, -1);

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
    startup_screen();
    vTaskDelay(500 / portTICK_PERIOD_MS);
    ssd1306_clear_screen(&g_oled, false);

    char buffer1[22] = "";
    char buffer2[22] = "";
    char buffer3[22] = "";
    for(;;)
    {
        sprintf(buffer1, "max: %d", g_dados.vbr_max);
        sprintf(buffer2, "min: %d", g_dados.vbr_mim);
        sprintf(buffer3, "rms: %d", g_dados.vbr_rms);

        ssd1306_clear_line(&g_oled, 3, false);
        ssd1306_display_text(&g_oled, 3, buffer1, 22, false);

        ssd1306_clear_line(&g_oled, 5, false);
        ssd1306_display_text(&g_oled, 5, buffer2, 22, false);

        ssd1306_clear_line(&g_oled, 7, false);
        ssd1306_display_text(&g_oled, 7, buffer3, 22, false);


        vTaskDelay(66 / portTICK_PERIOD_MS);
    }
}