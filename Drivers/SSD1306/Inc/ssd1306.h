#ifndef __SSD1306_H__
#define __SSD1306_H__

#include "stm32f1xx_hal.h"

#define SSD1306_I2C_ADDR        (0x3C << 1)

#define SSD1306_COMMAND         0x00
#define SSD1306_DATA            0x40

#define SSD1306_WIDTH           128
#define SSD1306_HEIGHT          32

void SSD1306_Init(void);
void SSD1306_UpdateScreen(void);
void SSD1306_DrawPixel(uint8_t x, uint8_t y);

#endif // __SSD1306_H__