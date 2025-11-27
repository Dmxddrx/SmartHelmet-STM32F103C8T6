#include "oled.h"

static I2C_HandleTypeDef *oled_i2c;

void OLED_Init(I2C_HandleTypeDef *hi2c) {
    oled_i2c = hi2c;
    SSD1306_Init();
}

void OLED_Clear(void) {
    SSD1306_Fill(SSD1306_COLOR_BLACK);
    SSD1306_UpdateScreen();
}

void OLED_Print(uint8_t x, uint8_t y, const char *str) {
	SSD1306_GotoXY(x, y);
	SSD1306_Puts((char*)str, &Font_7x10, SSD1306_COLOR_WHITE);
}

void OLED_Update(void){
	SSD1306_UpdateScreen();
}

void OLED_ClearArea(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
    SSD1306_DrawFilledRectangle(x, y, w, h, SSD1306_COLOR_BLACK);
}

void OLED_Rectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
    SSD1306_DrawFilledRectangle(x, y, w, h, SSD1306_COLOR_WHITE);
}
