#include "i2c_bus_mng.h"

#include <driver/i2c.h>
#include <esp_log.h>


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
	
	esp_err_t ret_i2c = i2c_driver_install(I2C_PORT, I2C_MODE_MASTER, 0, 0, 0);
	if(ESP_OK != ret_i2c) 
	{
		if(ESP_ERR_INVALID_ARG == ret_i2c)
		{
        	ESP_LOGE(TAG, "Driver i2c, parâmetros inválidos");
			
		}
		else
        	ESP_LOGE(TAG, "Driver i2c, já instalado");
		return 1;
    }
	ESP_LOGI(TAG, "I2C BUS inicializado");
	
    return 0;
}