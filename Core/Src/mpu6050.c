#include "mpu6050.h"
#include "math.h"

#define MPU6050_ADDR 0x68<<1

// Global variables for Live Expressions
I2C_HandleTypeDef *mpu_i2c;
int16_t raw_ax, raw_ay, raw_az;
int16_t raw_gx, raw_gy, raw_gz;
float ax, ay, az;
float gx, gy, gz;
AccidentLevel accidentLevel;

void MPU6050_Init(I2C_HandleTypeDef *hi2c) {
    mpu_i2c = hi2c;
    uint8_t data[2] = {0x6B, 0x00}; // PWR_MGMT_1 = 0
    HAL_I2C_Master_Transmit(mpu_i2c, MPU6050_ADDR, data, 2, 100);
}

void MPU6050_ReadAccelGyro(float *ax, float *ay, float *az, float *gx, float *gy, float *gz) {
    uint8_t reg = 0x3B;
    uint8_t buf[14];
    HAL_I2C_Master_Transmit(mpu_i2c, MPU6050_ADDR, &reg, 1, 100);
    HAL_I2C_Master_Receive(mpu_i2c, MPU6050_ADDR, buf, 14, 100);

    int16_t raw_ax = (buf[0]<<8)|buf[1];
    int16_t raw_ay = (buf[2]<<8)|buf[3];
    int16_t raw_az = (buf[4]<<8)|buf[5];
    int16_t raw_gx = (buf[8]<<8)|buf[9];
    int16_t raw_gy = (buf[10]<<8)|buf[11];
    int16_t raw_gz = (buf[12]<<8)|buf[13];

    *ax = raw_ax / 16384.0f; // g
    *ay = raw_ay / 16384.0f;
    *az = raw_az / 16384.0f;

    *gx = raw_gx / 131.0f;   // deg/s
    *gy = raw_gy / 131.0f;
    *gz = raw_gz / 131.0f;
}

AccidentLevel MPU6050_CheckAccident(float ax, float ay, float az, float gx, float gy, float gz) {
    float accMag = sqrtf(ax*ax + ay*ay + az*az);

    if (accMag >= 3.5f) {
        // Severe: accMag >= 3.5f
        return ACCIDENT_SEVERE;
    } else if (accMag >= 2.7f) {
        // Moderate: 2.7f <= accMag < 3.5f
        return ACCIDENT_MODERATE;
    } else if (accMag >= 2.0f) {
        // Mild: 2.0f <= accMag < 2.7f
        return ACCIDENT_MILD;
    } else {
        // None: accMag < 2.0f
        return ACCIDENT_NONE;
    }

    return accidentLevel;
}
