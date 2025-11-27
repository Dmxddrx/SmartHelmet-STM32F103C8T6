#ifndef MPU6050_H
#define MPU6050_H

#include "main.h"
#include <stdbool.h>

typedef enum {
    ACCIDENT_NONE,
    ACCIDENT_MILD,
    ACCIDENT_MODERATE,
    ACCIDENT_SEVERE
} AccidentLevel;

void MPU6050_Init(I2C_HandleTypeDef *hi2c);
void MPU6050_ReadAccelGyro(float *ax, float *ay, float *az, float *gx, float *gy, float *gz);
AccidentLevel MPU6050_CheckAccident(float ax, float ay, float az, float gx, float gy, float gz);

#endif
