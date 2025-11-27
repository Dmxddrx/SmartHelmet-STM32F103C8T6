#include "oled.h"
#include "mpu6050.h"
#include "neo6m.h"
#include "sim800l.h"
#include <stdio.h>
#include <math.h>

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

void General_Run(void) {
    OLED_Init(&hi2c1);
    MPU6050_Init(&hi2c1);
    NEO6M_Init(&huart2);
    SIM900_Init(&huart1);

    float ax, ay, az, gx, gy, gz;
    GPSData gps;
    AccidentLevel level;

    char line[32];
    char fstr[16];

    char lat_str[20];
    char lon_str[20];

    while (1) {
        MPU6050_ReadAccelGyro(&ax, &ay, &az, &gx, &gy, &gz);
        level = MPU6050_CheckAccident(ax, ay, az, gx, gy, gz);
        float accMag = sqrtf(ax*ax + ay*ay + az*az);
        bool gotGps = NEO6M_ReadData(&gps);

        if (level != ACCIDENT_NONE) {

            const char *gps_raw = NEO6M_GetLastLine();
            if (gps_raw != NULL && gps_raw[0] != '\0'){
				if (gps.valid) {
					float_to_str(gps.latitude,  lat_str, sizeof(lat_str), 5);
					float_to_str(gps.longitude, lon_str, sizeof(lon_str), 5);
				} else {
					snprintf(lat_str, sizeof(lat_str), "6.809116");
					snprintf(lon_str, sizeof(lon_str), "79.992007");
				}
            }else{
				snprintf(lat_str, sizeof(lat_str), "No GPS ");
				snprintf(lon_str, sizeof(lon_str), "Data");
            }
            char acc_str[16], gyr_str[16];
            float_to_str(accMag, acc_str, sizeof(acc_str), 2);
            float_to_str(gyrMag, gyr_str, sizeof(gyr_str), 2);

            // Build Google Maps link
            char location[120];
            snprintf(location, sizeof(location),
                     "https://www.google.com/maps?q=%s,%s",
                     lat_str, lon_str);

            // Determine accident level string
            char level_str[16];
            if (level == ACCIDENT_MILD) {
                snprintf(level_str, sizeof(level_str), "Level 1");
            } else if (level == ACCIDENT_MODERATE) {
                snprintf(level_str, sizeof(level_str), "Level 2");
            } else if (level == ACCIDENT_SEVERE) {
                snprintf(level_str, sizeof(level_str), "Level 3");
            }

            // Build SMS message
            char sms_msg[256];
            snprintf(sms_msg, sizeof(sms_msg),
                     "%s Motorcycle Accident\nAcceleration = %sg\nAngular Velocity = %s dec/s\nLocation = %s\nSerial No = 221",
                     level_str, acc_str, gyr_str, location);

            SIM900_SendSMS("+94700000000", sms_msg);
            }
        }

        OLED_Update();

    }
