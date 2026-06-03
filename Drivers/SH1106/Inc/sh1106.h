/**
  * @file    SH1106.h
  * @author  JM
  * @brief   Driver library for SH1106 OLED display (128x32) using I2C for STM32F1.
  * @date    2026
  */

#ifndef __SH1106_H__
#define __SH1106_H__

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_i2c.h"

// 00111100 << 1 = 01111000 = addr [7:1] + r/w [0]
#define SH1106_I2C_ADDR        (0x3C << 1)

#define SH1106_COMMAND         0x00
#define SH1106_DATA            0x40

#define SH1106_WIDTH           128
#define SH1106_HEIGHT          64

typedef enum {
    PIXEL_OFF = 0x00,
    PIXEL_ON = 0x01
} SH1106_PixelState;

/**
  * @brief Initializes the display sending the appropiate I2C commands
  * @param None
  * @retval None
  */
void SH1106_Init(void);

/**
  * @brief Sends a command to the display via I2C
  * @param command: command to be sent
  * @retval None
  */
void SH1106_WriteCommand(uint8_t command);

/**
  * @brief Sends I2C data to the display
  * @param data: the address of the first byte
  * @param size: the number of bytes from data
  * @retval None
  */
void SH1106_WriteData(uint8_t *data, uint16_t size);

/**
  * @brief Writes the content of the frame buffer into de GDDRAM
  * @param None
  * @retval None
  */
void SH1106_UpdateScreen(void);

/**
  * @brief Turns on/off the selected pixel.
  * The byte index in the buffer is calculated using:
  * * index = x + (y / 8) * 128
  *
  * Meaning: The page number (y / 8, as each page is 8 pixels high) 
  * is multiplied by the page width (128 bytes) to skip all previous 
  * pages. Then, the column offset (x) is added to find the exact byte.
  *
  * @param x: column [0, 127]
  * @param y: row [0, 31]
  * @param state: PIXEL_ON (0x01) or PIXEL_OFF (0x00)
  * @retval None
  */
void SH1106_DrawPixel(uint8_t x, uint8_t y, SH1106_PixelState state);

/**
  * @brief Draws a monochrome bitmap on the screen
  * @param x: starting column [0, 127]
  * @param y: starting row [0, 31]
  * @param bitmap: pointer to the image array
  * @param width: image width in pixels
  * @param height: image height in pixels
  * @retval None
  */
void SH1106_DrawBitmap(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t width, uint8_t height);

/**
  * @brief Writes a letter from a font map into the display.
  * @param index: the position of the letter in the map
  * @param x: column [0, 127]
  * @param y: row [0, 31]
  * @retval None
  * @version WIP
  */
void SH1106_WriteLetter(uint8_t fontmap[][5], uint8_t index, uint8_t x, uint8_t y);

/**
  * @brief Clears the display
  * @param None
  * @retval None
  */
void SH1106_Clear(void);

#endif // __SH1106_H__