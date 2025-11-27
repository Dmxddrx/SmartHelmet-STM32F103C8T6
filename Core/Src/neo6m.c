#include "neo6m.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

static UART_HandleTypeDef *gps_uart;

#define GPS_RX_BUF_SIZE 256
#define GPS_LINE_BUF_SIZE 160

static uint8_t gps_rx_buf[GPS_RX_BUF_SIZE];
static char gps_line_buf[GPS_LINE_BUF_SIZE];
static volatile bool gps_line_ready = false;

void NEO6M_Init(UART_HandleTypeDef *huart) {
    gps_uart = huart;
    HAL_UART_Receive_DMA(gps_uart, gps_rx_buf, GPS_RX_BUF_SIZE);
    __HAL_UART_ENABLE_IT(gps_uart, UART_IT_IDLE);
}

void NEO6M_UART_IdleHandler(UART_HandleTypeDef *huart) {
    if (huart->Instance == gps_uart->Instance) {
        __HAL_UART_CLEAR_IDLEFLAG(huart);
        HAL_UART_DMAStop(gps_uart);
        int len = GPS_RX_BUF_SIZE - __HAL_DMA_GET_COUNTER(gps_uart->hdmarx);
        if (len > GPS_LINE_BUF_SIZE - 1) len = GPS_LINE_BUF_SIZE - 1;
        memcpy(gps_line_buf, gps_rx_buf, len);
        gps_line_buf[len] = '\0';
        gps_line_ready = true;
        HAL_UART_Receive_DMA(gps_uart, gps_rx_buf, GPS_RX_BUF_SIZE);
    }
}

bool NEO6M_ReadData(GPSData *data) {
    if (!gps_line_ready) return false;
    gps_line_ready = false;
    char *line = gps_line_buf;
    char *sentence = strstr(line, "$GPRMC");
    if (!sentence) return false;
    char *end = strstr(sentence, "\r\n");
    if (end) *end = '\0';
    char *fields[20] = {0};
    int i = 0;
    char *tok = strtok(sentence, ",");
    while (tok && i < 20) {
        fields[i++] = tok;
        tok = strtok(NULL, ",");
    }

    if (i < 7) {
        data->valid = false;
        return true;
    }

    int fix = (fields[2][0] == 'A') ? 1 : 0;

    if (fix > 0) {
        float rawLat = atof(fields[3]);
        char ns = fields[4][0];
        float rawLon = atof(fields[5]);
        char ew = fields[6][0];
        data->latitude  = NEO6M_Convert(rawLat);
        data->longitude = NEO6M_Convert(rawLon);
        if (ns == 'S') data->latitude *= -1;
        if (ew == 'W') data->longitude *= -1;
        data->valid = true;
        return true;
    }

    data->valid = false;
    return true;
}

float NEO6M_Convert(float raw) {
    int degrees = (int)(raw / 100);
    float minutes = raw - (degrees * 100);
    return (float)degrees + minutes / 60.0f;
}

