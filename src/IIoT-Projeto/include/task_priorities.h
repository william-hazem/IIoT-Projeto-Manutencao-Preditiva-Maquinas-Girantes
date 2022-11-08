/**
 * @file task_priorities.h
 * @authors William Henrique    (william.martins@ee.ufcg.edu.br)
 *          Fabio   Victor      (fabio.nascimento@ee.ufcg.edu.br)
 * @brief   Regras de negocio para gerenciamento das tasks, definição de 
 * @version 0.1
 * @date 2022-11-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef __TASK_PRIORITIES_H
#define __TASK_PRIORITIES_H

#include <freertos/task.h>

/**
 * @brief Prioridades das tarefas
 */
typedef enum TASK_PRIORITIES
{
    DISPLAY_DATA = 1,
    AQUISITION_PRIORITY = 10,
    TEMP_PRIORITY = 9,
}TASK_PRIORITIES;

/**
 * @brief Lista de tarefas
 */
typedef enum TASK_IDS
{
    DISPLAY = 0,
    AQUISITION,
    TEMP,
}TASK_IDS;

/**
 * @brief Tabela de frequências das tarefas
 * 
 */
const static int TASK_FREQS[] = {
    10,     // display
    300,    // aquisition
    1,
};

/* Manipulador global de tarefas (MGT) */

TaskHandle_t gth_display;
TaskHandle_t gth_update_hora;
TaskHandle_t gth_aquisition;
TaskHandle_t gth_run;
TaskHandle_t gth_temp;

/**
 * @brief Recupera o período em milisegundos
 */
#define TASK_PERIOD(id) (int)((float)(1.0f / TASK_FREQS[id]) * 1000u)


#endif //!__TASK_PRIORITIES_H