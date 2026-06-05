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
#include <stdio.h>
#include <stdarg.h>
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

typedef enum {
  START_H = 0x00,
  CENTER_H = 0x01,
  END_H = 0x02
} SH1106_HorizontalAlign;

typedef enum {
  START_V = 0x00,
  CENTER_V = 0x01,
  END_V = 0x02
} SH1106_VerticalAlign;


void SH1106_Init(void);
void SH1106_Update_Screen(void);
void SH1106_Clear(void);
void SH1106_Draw_Pixel(uint8_t x, uint8_t y, SH1106_PixelState state);
void SH1106_Draw_Bitmap(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t width, uint8_t height);
void SH1106_Printf(SH1106_HorizontalAlign h_align, SH1106_VerticalAlign v_align, const char *format, ...);

#endif // __SH1106_H__