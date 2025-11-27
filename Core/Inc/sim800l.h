#ifndef SIM800L_H
#define SIM800L_H

#include "main.h"

void SIM900_Init(UART_HandleTypeDef *huart);
void SIM900_SendSMS(const char *number, const char *message);

#endif
