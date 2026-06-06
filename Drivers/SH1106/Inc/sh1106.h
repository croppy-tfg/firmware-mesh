/**
  * @file    sh1106.h
  * @author  JM
  * @brief   Driver library for SH1106 OLED display (128x64) using I2C for STM32F1.
  * @date    2026
  */

#ifndef __SH1106_H__
#define __SH1106_H__

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_i2c.h"
#include "cmsis_os.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// 00111100 << 1 = 01111000 = addr [7:1] + r/w [0]
#define SH1106_I2C_ADDR             (0x3C << 1)

#define SH1106_COMMAND_ADDR         0x00
#define SH1106_DATA_ADDR            0x40

#define SH1106_WIDTH                128
#define SH1106_HEIGHT               64
#define SH1106_BUFFER_SIZE          (SH1106_WIDTH * SH1106_HEIGHT / 8)

typedef enum {
  PIXEL_OFF = 0x00,
  PIXEL_ON = 0x01
} SH1106_PixelState;

void SH1106_Init(void);
void SH1106_Update_Screen(void);
void SH1106_Clear(void);

void SH1106_Draw_Text(uint8_t x, uint8_t y, SH1106_PixelState state, const char *text);
void SH1106_Draw_Pixel(uint8_t x, uint8_t y, SH1106_PixelState state);
void SH1106_Draw_Bitmap(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t width, uint8_t height);
void SH1106_Draw_Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SH1106_PixelState state);
void SH1106_Draw_Rectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, SH1106_PixelState state);

#endif // __SH1106_H__