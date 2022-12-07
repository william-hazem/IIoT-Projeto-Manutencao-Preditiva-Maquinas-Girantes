#include "datalogger.h"

#include <ds1307.h>
#include <time.h>

#include <i2c_bus_mng.h>

#include <esp_log.h>
#include <esp_err.h>

#include <driver/spi_master.h>
#include <sdmmc_cmd.h>
#include <esp_vfs_fat.h>

#define TAG "Datalogger"


sdmmc_host_t host = SDSPI_HOST_DEFAULT();
sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
spi_bus_config_t bus_cfg = {
    .mosi_io_num = SPI_MOSI,
    .miso_io_num = SPI_MISO,
    .sclk_io_num = SPI_CLK,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = 4000
};

/**
 * @brief estrutura do cartão MicroSD 
 */
sdmmc_card_t *card;

/**
 * @brief Inicializa a interface SPI e inicializa o driver SDMMC para comunicação
 *  com o módulo cartão microSD
 * 
 * @return ESP_OK caso inicializado
 */
esp_err_t sdcard_init();


int8_t datalogger_init()
{
    ds1307_init_desc(&dev, I2C_PORT, I2C_SDA, I2C_SDL);
    esp_err_t ret_sdcard = sdcard_init();
    if(ret_sdcard == ESP_OK)
    {
        ESP_LOGI(TAG, "* cartão sd inicializado");
    }
    else
    {
        ESP_LOGE(TAG, "* cartão sd não inicializado [crítico]");
    }
    return 0;
}

struct tm getTime(void)
{

    struct tm data;
    
    ds1307_get_time(&dev, &data);
    ESP_LOGI(pcTaskGetTaskName(0), "%04d-%02d-%02d %02d:%02d:%02d",
                 data.tm_year, data.tm_mon + 1,
                 data.tm_mday, data.tm_hour, data.tm_min, data.tm_sec);

    data.tm_mon += 1;
   
    return data;    
}


uint8_t setTime(time_t time)
{
    while(xSemaphoreTake(i2c_mutex, 10) != 1) {vTaskDelay(5);}

    struct tm data = {
        .tm_year = 2022,
        .tm_mon  = 10,  // 0-based
        .tm_mday = 05,
        .tm_hour = 15,
        .tm_min  = 5,
        .tm_sec  = 7
    };
    esp_err_t ret = ds1307_set_time(&dev, &data);
    xSemaphoreGive(i2c_mutex);
    return ret;

}

esp_err_t sdcard_init()
{
    esp_err_t ret = ESP_OK;

    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    const char mount_point[] = MOUNT_POINT;
    ESP_LOGI("SDcard_Init", "Initializing SD card");

    // Use settings defined above to initialize SD card and mount FAT filesystem.
    // Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions.
    // Please check its source code and implement error recovery when developing
    // production applications.
    ESP_LOGI("SDcard_Init", "Using SPI peripheral");

    
    host.max_freq_khz = SDMMC_FREQ_PROBING;
    ret = spi_bus_initialize( host.slot, &bus_cfg, SPI_DMA_CHAN);

    
    if (ret != ESP_OK) {
        ESP_LOGE("SDcard_Init", "Failed to initialize bus.");
        return ESP_FAIL;
    }

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.

    slot_config.gpio_cs = SPI_CS;
    slot_config.host_id = host.slot;


    ESP_LOGI("SDcard_Init", "Mounting filesystem");
    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);
    // ret = sdmmc_card_init(&host, card);
    if (ret != ESP_OK) {
        spi_bus_free(host.slot);
        if (ret == ESP_FAIL) {
            ESP_LOGE("SDcard_Init", "Failed to mount filesystem. "
                     "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE("SDcard_Init", "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }
    ESP_LOGI("SDcard_Init", "Filesystem mounted");

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);

    return ESP_OK;
}

esp_err_t sdcard_unmount() {
    /// desmonta a partição e desabilita o periférico SPI
    esp_err_t ret = esp_vfs_fat_sdcard_unmount(MOUNT_POINT, card);
    
    if(ESP_OK != ret)
        ESP_LOGE(TAG, "Erro ao desmontar partição (%s)", esp_err_to_name(ret));
    
    ESP_LOGI(TAG, "Cartão desmontado");

    /// libera o barramento SPI, após todos os módulos dependentes terem sido removidos
    spi_bus_free(host.slot);

    return ESP_OK;    
}