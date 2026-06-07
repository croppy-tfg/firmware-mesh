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
    SH1106_PIXEL_OFF = 0x00,
    SH1106_PIXEL_ON = 0x01
} SH1106_Pixel_State_t;

typedef enum {
    SH1106_MIRROR_NONE = 0,
    SH1106_MIRROR_HORIZONTAL = 0x01,
    SH1106_MIRROR_VERTICAL   = 0x02,
    SH1106_MIRROR_BOTH       = 0x03
} SH1106_MirrorMode_t;

typedef enum {
    SH1106_PUMP_6_4V = 0x30,
    SH1106_PUMP_7_4V = 0x31,
    SH1106_PUMP_8_0V = 0x32, // POR
    SH1106_PUMP_9_0V = 0x33
} SH1106_PumpVoltage_t;

typedef enum {
    SH1106_COM_SEQUENTIAL = 0x02,
    SH1106_COM_ALTERNATIVE  = 0x12 // POR
} SH1106_ComConfig_t;

typedef struct {
    I2C_HandleTypeDef    *hi2c;                 // pointer to I2C handle
    uint16_t             i2c_addr;              // I2C address
    uint8_t              contrast;              // constrast [0x00, 0xFF]
    uint8_t              multiplex_ratio;       // multiplex ratio [0x00, 0x3F]
    uint8_t              display_offset;        // vertical offset [0x00, 0x3F]
    uint8_t              start_line;            // start line [0x40, 0x7F]
    uint8_t              osc_freq_div_ratio;           // clock divide ratio
    SH1106_PumpVoltage_t pump_voltage;          // internal DC-DC voltage 
    SH1106_ComConfig_t   com_pins_config;       // COM pins (rows) configuration
    SH1106_MirrorMode_t  mirror;                // H/V mirror
} SH1106_Config_t;

void SH1106_Init(const SH1106_Config_t *config);
void SH1106_Update_Screen(void);
void SH1106_Clear(void);

void SH1106_Draw_Text(uint8_t x, uint8_t y, SH1106_Pixel_State_t state, const char *text);
void SH1106_Draw_Pixel(uint8_t x, uint8_t y, SH1106_Pixel_State_t state);
void SH1106_Draw_Bitmap(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t width, uint8_t height);
void SH1106_Draw_Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SH1106_Pixel_State_t state);
void SH1106_Draw_Rectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, SH1106_Pixel_State_t state);

#endif // __SH1106_H__