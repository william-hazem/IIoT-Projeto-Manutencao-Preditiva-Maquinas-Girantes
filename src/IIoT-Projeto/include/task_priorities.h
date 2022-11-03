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

typedef enum TASK_PRIORITIES
{
    DISPLAY_DATA = 1,
    AQUISITION_PRIORITY = 10,
}TASK_PRIORITIES;

typedef enum TASK_IDS
{
    DISPLAY = 1,
    AQUISITION,
    RUN,
}TASK_IDS;

const int TASK_FREQS[] = {
    15,
    300,
    10,
};

TaskHandle_t gth_display;
TaskHandle_t gth_aquisition;
TaskHandle_t gth_run;



#endif //!__TASK_PRIORITIES_H