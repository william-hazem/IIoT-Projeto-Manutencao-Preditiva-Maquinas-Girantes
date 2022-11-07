#include "rms.h"
/**
 * @file rms.c
 * @author william henrique (william.martins@ee.ufcg.edu.br)
 * @brief Calcula o valor rms de um conjunto com n amostras
 * @version 0.1
 * @date 2022-09-27
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <math.h>
#include <stdio.h>
float rms(int* s, int n)
{
    float rmsval = 0;
    int* p = s;

    /// soma o quadrado dos termos
    while(p != (s+n)) 
    {
        rmsval += *(p)**(p);
        p++;
    }
    /// devolve o rms calculado
    return sqrt(rmsval/n);
}

float rmsf(float* s, int n)
{
    float rmsval = 0;
    float* p = s;

    /// soma o quadrado dos termos
    while(p != (s+n)) 
    {
        rmsval += (*p)*(*p);
        p++;
    }
    /// devolve o rms calculado
    return sqrt(rmsval/n);
}