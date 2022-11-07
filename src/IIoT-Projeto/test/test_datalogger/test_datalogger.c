#include <unity_test_runner.h>
#include <unity.h>

#include <esp_err.h>
#include <datalogger.h>
#include <time.h>
#include <memory.h>


#include <sdmmc_cmd.h>
#include <esp_vfs_fat.h>

#define SPI_CLK     GPIO_NUM_21
#define SPI_MISO    GPIO_NUM_19
#define SPI_MOSI    GPIO_NUM_18
#define SPI_CS      GPIO_NUM_5
#define SPI_DMA_CHAN 1

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


int8_t sdcard_init();

i2c_dev_t dev;

void test_ds1307_write()
{
    esp_err_t ret = ds1307_init_desc(&dev, 0, 23, 22);
    TEST_ASSERT_EQUAL(ret, ESP_OK);
    struct tm data = {
        .tm_year = 2022,
        .tm_mon  = 10,  // 0-based
        .tm_mday = 05,
        .tm_hour = 15,
        .tm_min  = 5,
        .tm_sec  = 7
    };
    ret = ds1307_set_time(&dev, &data);
    TEST_ASSERT_EQUAL(ret, ESP_OK);
}


void test_ds1307_read()
{
    struct tm data = {
        .tm_year = 2022,
        .tm_mon  = 10,  // 0-based
        .tm_mday = 05,
        .tm_hour = 15,
        .tm_min  = 5,
        .tm_sec  = 7
    };
    esp_err_t ret = ds1307_set_time(&dev, &data);
    TEST_ASSERT_EQUAL(ret, ESP_OK);

    struct tm read;
    ret = ds1307_get_time(&dev, &read);
    ets_printf("ret %s\n", esp_err_to_name(ret));
    ets_printf("%02d %02d\n", read.tm_hour, data.tm_hour);
    ets_printf("%02d %02d\n", read.tm_min, data.tm_min);
    ets_printf("%02d %02d\n", read.tm_sec, data.tm_sec);
    ets_printf("%02d %02d\n", read.tm_year, data.tm_year);
    ets_printf("%02d %02d\n", read.tm_mon, data.tm_mon);
    ets_printf("%02d %02d\n", read.tm_mday, data.tm_mday);
    

    TEST_ASSERT_EQUAL(read.tm_hour, data.tm_hour);
    TEST_ASSERT_EQUAL(read.tm_min, data.tm_min);
    TEST_ASSERT_EQUAL(read.tm_sec, data.tm_sec);
}

void test_sdcard()
{
    sdcard_init();

    char caminhoArquivo[32] = MOUNT_POINT"/";
    char buffer[124] = "";
    strcat(caminhoArquivo, "teste.txt");

    FILE *arquivo = NULL;
    
    /// Teste de escrita
    arquivo = fopen(caminhoArquivo, "w");
    TEST_ASSERT(arquivo != NULL);

    fprintf(arquivo, "Hello World - Teste SD");

    fclose(arquivo);


    /// Teste de leitura
    arquivo = fopen(caminhoArquivo, "r");
    TEST_ASSERT(arquivo != NULL);

    fread(buffer, 120, 1, arquivo);
    size_t nlido = strlen(buffer);
    
    TEST_ASSERT(nlido > 0);
    TEST_ASSERT(strcmp( "Hello World - Teste SD", buffer) == 0);

    fclose(arquivo);

    /// desmonta a partição e desabilita o periférico SPI
    esp_err_t ret = esp_vfs_fat_sdcard_unmount(MOUNT_POINT, card);
    
    if(ESP_OK != ret)
        ESP_LOGE(TAG, "Erro ao desmontar partição (%s)", esp_err_to_name(ret));
    
    ESP_LOGI(TAG, "Cartão desmontado");

    /// libera o barramento SPI, após todos os módulos dependentes terem sido removidos
    spi_bus_free(host.slot);
}

int app_main(int argc, char **argv)
{

    i2c_config_t i2c_config = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = 23,
		.scl_io_num = 22,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		.master.clk_speed = 400000
	};
	ESP_ERROR_CHECK(i2c_param_config(0, &i2c_config));
    i2c_driver_install(0, I2C_MODE_MASTER, 0, 0, 0);


    UNITY_BEGIN();
    
    RUN_TEST(test_ds1307_write);
    RUN_TEST(test_ds1307_read);
    RUN_TEST(test_sdcard);
    
    UNITY_END();

    while(true){vTaskDelay(1);}
    return 0;
}


int8_t sdcard_init()
{
     esp_err_t ret = ESP_OK;

    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
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

    if (ret != ESP_OK) {
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