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


/**
 * @brief Inicializa e aloca memória para utilizar o driver i2c
 * 
 * @return 0 Inicializado
 *         1 Erro 
 */
uint8_t i2c_bus_init();

#endif //! __I2C_BUS_MNG_H