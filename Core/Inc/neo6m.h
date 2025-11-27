#ifndef NEO6M_H
#define NEO6M_H

#include "main.h"
#include <stdbool.h>

typedef struct {
    float latitude;
    float longitude;
    bool valid;
} GPSData;

void NEO6M_Init(UART_HandleTypeDef *huart);
bool NEO6M_ReadData(GPSData *data);
bool NEO6M_GetRawLatLon(char *out, size_t out_len);
float NEO6M_Convert(float raw);

#endif
