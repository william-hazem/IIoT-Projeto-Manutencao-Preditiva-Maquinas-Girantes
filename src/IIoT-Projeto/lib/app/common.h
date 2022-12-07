/**
 * @file common.h
 * @author your name (you@domain.com)
 * @brief Contêm informações comuns de projeto desde parâmetros de builds a superglobais
 * @version 0.1
 * @date 2022-11-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef __COMMON_H
#define __COMMON_H

#define APP_RUN 0x4              // Estado de execução

#if APP_RUN ==2                   // habilita configurações de teste
#   define TESTE
#elif APP_RUN & 0x4            // habilita o log de status no console
# define STATUS
#endif

#define WAKEUP_PERIOD 10000000U // Periodo de aquisição dos dados

#define BROKER_URI "broker.emqx.io"
#define W2P        "/grupoZ"         /// onde publicar

/// @brief Dados da aplicação

typedef struct app_t {
    float vbr_max, vbr_mim, vbr_rms;
    float *vbr_begin, *vbr_end;
    int vbr_count;
    float *vbr_i, *vbr_j;
    char hora[32];
    float temperatura;
} app_t;

app_t g_dados;


#endif //! __COMMON_H