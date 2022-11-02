/**
 * @file vibration.h
 * @authors William Henrique    (william.martins@ee.ufcg.edu.br)
 *          Fabio   Victor      (fabio.nascimento@ee.ufcg.edu.br)
 * @brief Regras de aquisição de dados a partir do acelerômetro de 3-eixos
 * @version 0.1
 * @date 2022-11-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#ifndef __VIBRATION_H
#define __VIBRATION_H

#include <inttypes.h>

#include "common.h"

#ifndef TESTE
#   define VBR_MAX_BUFFER_SIZE 3000
#   define VBR_FREQ            330
#   define VBR_DELAY           1 / VBR_FREQ * 1000
#else
#   define VBR_MAX_BUFFER_SIZE 5
#   define VBR_FREQ            1
#   define VBR_DELAY           1 / VBR_FREQ * 1000
#   define VBR_DEFAULT_DATA    {2, 3, 1, 4, 5}
#endif

int16_t vbr_data[VBR_MAX_BUFFER_SIZE];
int16_t *vbr_head, *vbr_end;
int16_t vbr_min, vbr_max;

/**
 * @brief Inicialzia os recursos necessários para realizar a instrumentação
 *  de vibração do sistema de aquisição, inicializa os dados com valor zero na memória
 * 
 * @return true 
 * @return false 
 */
uint8_t vbr_init();

void vbr_task(void *args);


#endif //!__VIBRATION_H