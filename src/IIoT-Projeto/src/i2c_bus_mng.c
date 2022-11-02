#include "i2c_bus_mng.h"

#include <driver/i2c.h>
#include <esp_log.h>

#define TAG "I2C BUS MANAGER"

uint8_t i2c_bus_init()
{
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
        ESP_LOGI(TAG, "Driver i2c, já instalado");
    }

    return 0;
}