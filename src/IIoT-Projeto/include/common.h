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

#define APP_RUN 0               // Estado de execução

#if APP_RUN ==2                   // habilita configurações de teste
#   define TESTE
#endif

#define WAKEUP_PERIOD 10000000U // Periodo de aquisição dos dados


#endif //! __COMMON_H