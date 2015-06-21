#ifndef __srv_h__
#define __srv_h__

#include "tp3.h"

void servidor(int mi_cliente);
/* Funciones auxiliares sobre arrays */
void initializeBoolBuffer(int* buffer, int size, int value);
int countActive(int* buffer, int count);

#endif
