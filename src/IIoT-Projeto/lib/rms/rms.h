/**
 * @file rms.c
 * @author william henrique (william.martins@ee.ufcg.edu.br)
 * @brief Calcula o valor rms de um conjunto com n amostras
 * @version 0.1
 * @date 2022-09-27
 * 
 * @copyright Copyright (c) 2022
 */

/**
 * @brief Calcula o RMS de um série de pontos
 * 
 * @param s     set - conjunto de inteiros
 * @param n     número de amostras
 * @return float rms calculado
 */
float rms(int* s, int n);

/**
 * @brief Calcula o RMS de um série de pontos
 * 
 * @param s     set - conjunto de pontos flutuantes
 * @param n     número de amostras
 * @return float rms calculado
 */
float rmsf(float* s, int n);