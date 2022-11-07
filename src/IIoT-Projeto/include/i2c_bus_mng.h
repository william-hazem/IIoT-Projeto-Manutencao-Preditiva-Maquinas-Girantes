/**
 * @file i2c_bus_mng.h
 * @author William Henrique    (william.martins@ee.ufcg.edu.br)
*          Fabio   Victor      (fabio.nascimento@ee.ufcg.edu.br)
 * @brief Gerencia o barramento i2c
 * @version 0.1
 * @date 2022-11-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#ifndef __I2C_BUS_MNG_H
#define __I2C_BUS_MNG_H


#include <inttypes.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>



#define I2C_SDL 22
#define I2C_SDA 23
#define I2C_FREQ 400000
#define I2C_PORT I2C_NUM_0


SemaphoreHandle_t i2c_mutex;

/**
 * @brief Inicializa e aloca memória para utilizar o driver i2c
 * 
 * @return 0 Inicializado
 *         1 Erro 
 */
uint8_t i2c_bus_init();

/**
 * @brief Aguarda até que o canal i2c esteja livre
 * @param x Ação
 * @param t Tempo de espera em ticks
 */
#define I2C_WAIT_UNTIL_FREE(x, t) \
    while(xSemaphoreTake(i2c_mutex, ( TickType_t ) t ) != pdTRUE) {;} \
    x;                                                                \
    xSemaphoreGive(i2c_mutex); 


#endif //! __I2C_BUS_MNG_H