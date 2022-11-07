#include "i2c_bus_mng.h"

#include <driver/i2c.h>
#include <esp_log.h>

#include "common.h"

#define TAG "I2C BUS MANAGER"

uint8_t i2c_bus_init()
{

	i2c_mutex = xSemaphoreCreateMutex();
	ESP_ERROR_CHECK(i2c_mutex == NULL);

    i2c_config_t i2c_config = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = I2C_SDA,
		.scl_io_num = I2C_SDL,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		.master.clk_speed = I2C_FREQ
	};
	ESP_ERROR_CHECK(i2c_param_config(I2C_PORT, &i2c_config));
	
	if(ESP_OK != i2c_driver_install(I2C_PORT, I2C_MODE_MASTER, 0, 0, 0)){
        ESP_LOGI(TAG, "Driver i2c, já instalado");
    }

    return 0;
}