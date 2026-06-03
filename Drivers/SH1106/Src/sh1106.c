#include "sh1106.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_def.h"
#include "stm32f1xx_hal_i2c.h"

extern I2C_HandleTypeDef hi2c1;

/**
 * @brief The size of the display is 128x32 px. The display data 
 * organization consists in 4 pages of 128x8 px each, so the buffer 
 * below needs to be 512B long to store all the pages.
 */
static uint8_t SH1106_Buffer[512];

// Simple font map to print a simple message (Hola Lau)
const uint8_t Font5x7_Simple[][5] = {
    {0x7C, 0x08, 0x08, 0x08, 0x7C}, // H (Índice 0)
    {0x38, 0x44, 0x44, 0x44, 0x38}, // o (Índice 1)
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // l (Índice 2)
    {0x7C, 0x12, 0x11, 0x12, 0x7C}, // a (Índice 3)
    {0x00, 0x00, 0x00, 0x00, 0x00}, // Espacio (Índice 4)
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // L (Índice 5)
    {0x3C, 0x40, 0x40, 0x40, 0x3C}  // u (Índice 6)
};

void SH1106_Init(void)
{
    HAL_Delay(100);

    SH1106_WriteCommand(0xAE); // Display OFF

    SH1106_WriteCommand(0xD5); // Display Clock Divide Ratio
    SH1106_WriteCommand(0x80); // \_ 0x80 (Default value)

    SH1106_WriteCommand(0xA8); // Multiplex Ratio
    SH1106_WriteCommand(0x3F); // \_ 0x1F (32 rows)

    SH1106_WriteCommand(0xD3); // Display Offset
    SH1106_WriteCommand(0x00); // \_ No offset

    SH1106_WriteCommand(0x40); // Display Start Line: 0

    SH1106_WriteCommand(0x8D); // Charge Pump
    SH1106_WriteCommand(0x14); // \_ Enable charge pump

    SH1106_WriteCommand(0xA1); // Mirror H

    SH1106_WriteCommand(0xC8); // Mirror V

    SH1106_WriteCommand(0xDA); // COM Pins Hardware Config
    SH1106_WriteCommand(0x02); // 0x02 -> sequential and disable remap

    SH1106_WriteCommand(0x81); // Contrast Control
    SH1106_WriteCommand(0x8F); // \_ Medium-High brightness

    SH1106_WriteCommand(0xD9); // Pre-charge Period
    SH1106_WriteCommand(0xF1); // \_ 0xF1

    SH1106_WriteCommand(0xDB); // VCOMH Deselect Level
    SH1106_WriteCommand(0x40); // \_ 0x40

    SH1106_WriteCommand(0xA4); // The output will follow the GDDRAM content

    SH1106_WriteCommand(0xA6); // Normal Display (1 ON, 0 OFF)

    SH1106_WriteCommand(0xAF); // Display ON
}

void SH1106_WriteCommand(uint8_t command)
{
    // 0x00 -> sending a command
    HAL_I2C_Mem_Write(&hi2c1, SH1106_I2C_ADDR, 0x00, 1,
        &command, 1, HAL_MAX_DELAY);
}

void SH1106_WriteData(uint8_t *data, uint16_t size)
{
    // 0x40 -> sending data to GDDRAM
    HAL_I2C_Mem_Write(&hi2c1, SH1106_I2C_ADDR, 0x40, 1,
            data, size, HAL_MAX_DELAY);
}

void SH1106_UpdateScreen(void) 
{
    for (uint8_t i = 0; i < 4; i++) {
        SH1106_WriteCommand(0xB0 + i); // Page Address (only in page addressing mode)
        
        SH1106_WriteCommand(0x00); // Lower Column Address
                                   // \_ 0000XXXX

        SH1106_WriteCommand(0x10); // Higher Column Address (0)
                                   // \_ 0001XXXX

        // Dumps the content of the buffer page by page (selected
        // by i * 128) to the display (128B each time)
        SH1106_WriteData(&SH1106_Buffer[SH1106_WIDTH * i], SH1106_WIDTH);
    }
}

void SH1106_DrawPixel(uint8_t x, uint8_t y, SH1106_PixelState state) 
{
    if (x >= 128 || y >= 32) return; // out of range
    if (state == PIXEL_ON)
        SH1106_Buffer[x + (y / 8) * 128] |= (1 << (y % 8));
    else
        SH1106_Buffer[x + (y / 8) * 128] &= ~(1 << (y % 8));
}

void SH1106_DrawBitmap(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t width, uint8_t height) 
{
    for (uint8_t i = 0; i < width; i++) {
        for (uint8_t j = 0; j < height; j++) {
            // calculate the bit inside the bitmap
            uint8_t byte_index = i + (j / 8) * width;
            uint8_t bit_index = j % 8;
            
            // if the bit is '1', the px is turned on
            if (bitmap[byte_index] & (1 << bit_index)) {
                SH1106_DrawPixel(x + i, y + j, PIXEL_ON);
            }
        }
    }
}

void SH1106_WriteLetter(uint8_t fontmap[][5], uint8_t index, uint8_t x, uint8_t y)
{
    for (uint8_t i = 0; i < 5; i++) { // each letter is 5B (5 columns)
        uint8_t column = fontmap[index][i];
        for (uint8_t j = 0; j < 8; j++) {
            if ((column >> j) & 0x01) { // only draw those pixels which are ON
                SH1106_DrawPixel(x + i, y + j, PIXEL_ON);
            }
        }
    }
}

void SH1106_Clear(void) 
{
    for (int i = 0; i < 512; i++) {
        SH1106_Buffer[i] = 0x00;
    }
}