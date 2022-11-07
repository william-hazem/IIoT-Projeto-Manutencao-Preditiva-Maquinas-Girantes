/**
 * @file oled.h
 * @author William Henrique    (william.martins@ee.ufcg.edu.br)
 *         Fabio   Victor      (fabio.nascimento@ee.ufcg.edu.br)
 * @brief Gerenciamento do display Oled ssd1306
 * @version 0.1
 * @date 2022-11-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef __OLED_H
#define __OLED_H

#include <ssd1306.h>

SSD1306_t g_oled;

typedef struct display_dados_t {
    float vbr_max, vbr_mim, vbr_rms;
    float *vbr_begin, *vbr_end;
    float *vbr_i, *vbr_j;
    char hora[32];
} display_dados_t;

display_dados_t g_dados;





/**
 * @brief Função auxiliar para centralizar textos na tela
 * 
 * @param text texto a ser exibido
 * @return char*
 */
char* alignCenter(char* text);

/**
 * @brief Inicializa o display oled
 * 
 */
void init_oled();

/**
 * @brief Tela de inicialização
 * 
 */
void startup_screen();


/**
 * @brief Reseta os dados amostrados no display
 * 
 */
void display_reset();


void display_task(void *args);

void update_hora_task(void* args);


#endif //!__OLED_H