#include <ssd1306.h>
#include <keypad.h>
#include <string.h>

#include <mpu6050/mpu6050.h>

#include <esp_err.h>

SSD1306_t g_oled;


char* alignCenter(char* text);

void init_oled();


void app_main() 
{
    init_oled();
    i2c_config_t i2c_config = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = 23,
		.scl_io_num = 22,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		.master.clk_speed = 40000
	};
	ESP_ERROR_CHECK(i2c_param_config(0, &i2c_config));
	
	if(!i2c_driver_install(0, I2C_MODE_MASTER, 0, 0, 0)){
        ESP_LOGI(tag, "Driver i2c, já instalado");
    }

    mpu6050_init();
    vTaskDelay(500 / portTICK_PERIOD_MS);
    
    mpu6050_acceleration_t data;
    ssd1306_clear_screen(&g_oled, false);
    if(mpu6050_test_connection())
        ets_printf("RESPONDING\n");
    
    char buffer[32] = "";
    
    ssd1306_display_text(&g_oled, 2, "  x     y    z  ", 22, false);
    for(;;)
    {
        
        mpu6050_get_acceleration(&data);
        
        ets_printf("%d, %d, %d\n", data.accel_x, data.accel_y, data.accel_z);

        sprintf(buffer, "%d %d %d", data.accel_x, data.accel_y, data.accel_z);
        ssd1306_clear_line(&g_oled, 4, false);
        ssd1306_display_text(&g_oled, 4, buffer, 22, false);
        vTaskDelay(50);
    }
}


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