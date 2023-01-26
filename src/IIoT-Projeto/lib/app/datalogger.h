/**
 * @file datalogger.h
 * @author William Henrique    (william.martins@ee.ufcg.edu.br)
 *         Fabio   Victor      (fabio.nascimento@ee.ufcg.edu.br)
 * @brief Gerencia o armazenamento de dados e acesso ao RTC
 * @version 0.1
 * @date 2022-11-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#ifndef __DATALOGGER_H
#define __DATALOGGER_H

#include <ds1307.h>

#define SPI_CLK     GPIO_NUM_18
#define SPI_MISO    GPIO_NUM_19
#define SPI_MOSI    GPIO_NUM_23
#define SPI_CS      GPIO_NUM_5
#define SPI_DMA_CHAN 1

/**
 * @brief Ponto de montagem do cartão SD
 */
#define MOUNT_POINT "/sdcard"

/**
 * @brief Dispositivo i2c para o ds1307 
 */
i2c_dev_t dev;

/**
 * @brief Inicializa os recursos I2C para o módulo DS1307 RTC e
 *  os recursos SPI para acesso ao cartão microSD
 * 
 * @return ESP_OK se todos os recursos forem inicializados 
 */
int8_t datalogger_init();

/**
 * @brief Salva os dados da pilha no cartão SD
 * 
 * @param args 
 */
void datalogger_task(void *args);

/**
 * @brief Recupera o tempo salvo no módulo RTC
 * 
 * @return struct tm 
 */
struct tm getTime(void);

/**
 * @brief Define o tempo no módulo RTC
 * 
 * @return ESP_OK se informação salva 
 */
uint8_t setTime();


#endif //! __DATALOGGER_H
