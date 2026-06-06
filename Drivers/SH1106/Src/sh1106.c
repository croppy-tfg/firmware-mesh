#include "sh1106.h"

extern I2C_HandleTypeDef hi2c1;

/**
 * @brief The size of the display is 128x64 px. The display data 
 * organization consists in 8 pages of 128x8 px each, so the buffer 
 * below needs to be 1024B long to store all the pages.
 */
static uint8_t SH1106_Buffer[SH1106_BUFFER_SIZE];

static const uint8_t SH1106_Font5x8[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // ' '
    {0x00, 0x00, 0x5f, 0x00, 0x00}, // !
    {0x00, 0x07, 0x00, 0x07, 0x00}, // "
    {0x14, 0x7f, 0x14, 0x7f, 0x14}, // #
    {0x24, 0x2a, 0x7f, 0x2a, 0x12}, // $
    {0x23, 0x13, 0x08, 0x64, 0x62}, // %
    {0x36, 0x49, 0x55, 0x22, 0x50}, // &
    {0x00, 0x05, 0x03, 0x00, 0x00}, // '
    {0x00, 0x1c, 0x22, 0x41, 0x00}, // (
    {0x00, 0x41, 0x22, 0x1c, 0x00}, // )
    {0x14, 0x08, 0x3e, 0x08, 0x14}, // *
    {0x08, 0x08, 0x3e, 0x08, 0x08}, // +
    {0x00, 0x50, 0x30, 0x00, 0x00}, // ,
    {0x08, 0x08, 0x08, 0x08, 0x08}, // -
    {0x00, 0x60, 0x60, 0x00, 0x00}, // .
    {0x20, 0x10, 0x08, 0x04, 0x02}, // /
    {0x3e, 0x51, 0x49, 0x45, 0x3e}, // 0
    {0x00, 0x42, 0x7f, 0x40, 0x00}, // 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
    {0x21, 0x41, 0x45, 0x4b, 0x31}, // 3
    {0x18, 0x14, 0x12, 0x7f, 0x10}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x3c, 0x4a, 0x49, 0x49, 0x30}, // 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x06, 0x49, 0x49, 0x29, 0x1e}, // 9
    {0x00, 0x36, 0x36, 0x00, 0x00}, // :
    {0x00, 0x56, 0x36, 0x00, 0x00}, // ;
    {0x08, 0x14, 0x22, 0x41, 0x00}, // <
    {0x14, 0x14, 0x14, 0x14, 0x14}, // =
    {0x00, 0x41, 0x22, 0x14, 0x08}, // >
    {0x02, 0x01, 0x51, 0x09, 0x06}, // ?
    {0x32, 0x49, 0x79, 0x41, 0x3e}, // @
    {0x7e, 0x11, 0x11, 0x11, 0x7e}, // A
    {0x7f, 0x49, 0x49, 0x49, 0x36}, // B
    {0x3e, 0x41, 0x41, 0x41, 0x22}, // C
    {0x7f, 0x41, 0x41, 0x22, 0x1c}, // D
    {0x7f, 0x49, 0x49, 0x49, 0x41}, // E
    {0x7f, 0x09, 0x09, 0x09, 0x01}, // F
    {0x3e, 0x41, 0x49, 0x49, 0x7a}, // G
    {0x7f, 0x08, 0x08, 0x08, 0x7f}, // H
    {0x00, 0x41, 0x7f, 0x41, 0x00}, // I
    {0x20, 0x40, 0x41, 0x3f, 0x01}, // J
    {0x7f, 0x08, 0x14, 0x22, 0x41}, // K
    {0x7f, 0x40, 0x40, 0x40, 0x40}, // L
    {0x7f, 0x02, 0x0c, 0x02, 0x7f}, // M
    {0x7f, 0x04, 0x08, 0x10, 0x7f}, // N
    {0x3e, 0x41, 0x41, 0x41, 0x3e}, // O
    {0x7f, 0x09, 0x09, 0x09, 0x06}, // P
    {0x3e, 0x41, 0x51, 0x21, 0x5e}, // Q
    {0x7f, 0x09, 0x19, 0x29, 0x46}, // R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // S
    {0x01, 0x01, 0x7f, 0x01, 0x01}, // T
    {0x3f, 0x40, 0x40, 0x40, 0x3f}, // U
    {0x1f, 0x20, 0x40, 0x20, 0x1f}, // V
    {0x3f, 0x40, 0x38, 0x40, 0x3f}, // W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X
    {0x07, 0x08, 0x70, 0x08, 0x07}, // Y
    {0x61, 0x51, 0x49, 0x45, 0x43}, // Z
    {0x00, 0x7f, 0x41, 0x41, 0x00}, // [
    {0x02, 0x04, 0x08, 0x10, 0x20}, // backslash
    {0x00, 0x41, 0x41, 0x7f, 0x00}, // ]
    {0x04, 0x02, 0x01, 0x02, 0x04}, // ^
    {0x40, 0x40, 0x40, 0x40, 0x40}, // _
    {0x00, 0x01, 0x02, 0x04, 0x00}, // `
    {0x20, 0x54, 0x54, 0x54, 0x78}, // a
    {0x7f, 0x48, 0x44, 0x44, 0x38}, // b
    {0x38, 0x44, 0x44, 0x44, 0x20}, // c
    {0x38, 0x44, 0x44, 0x48, 0x7f}, // d
    {0x38, 0x54, 0x54, 0x54, 0x18}, // e
    {0x08, 0x7e, 0x09, 0x01, 0x02}, // f
    {0x0c, 0x52, 0x52, 0x52, 0x3e}, // g
    {0x7f, 0x08, 0x04, 0x04, 0x78}, // h
    {0x00, 0x44, 0x7d, 0x40, 0x00}, // i
    {0x20, 0x40, 0x44, 0x3d, 0x00}, // j
    {0x7f, 0x10, 0x28, 0x44, 0x00}, // k
    {0x00, 0x41, 0x7f, 0x40, 0x00}, // l
    {0x7c, 0x04, 0x18, 0x04, 0x78}, // m
    {0x7c, 0x08, 0x04, 0x04, 0x78}, // n
    {0x38, 0x44, 0x44, 0x44, 0x38}, // o
    {0x7c, 0x14, 0x14, 0x14, 0x08}, // p
    {0x08, 0x14, 0x14, 0x18, 0x7f}, // q
    {0x7c, 0x08, 0x04, 0x04, 0x08}, // r
    {0x48, 0x54, 0x54, 0x54, 0x20}, // s
    {0x04, 0x3f, 0x44, 0x40, 0x20}, // t
    {0x3c, 0x40, 0x40, 0x20, 0x7c}, // u
    {0x1c, 0x20, 0x40, 0x20, 0x1c}, // v
    {0x3c, 0x40, 0x30, 0x40, 0x3c}, // w
    {0x44, 0x28, 0x10, 0x28, 0x44}, // x
    {0x0c, 0x50, 0x50, 0x50, 0x3c}, // y
    {0x44, 0x64, 0x54, 0x4c, 0x44}, // z
    {0x00, 0x08, 0x36, 0x41, 0x00}, // {
    {0x00, 0x00, 0x7f, 0x00, 0x00}, // |
    {0x00, 0x41, 0x36, 0x08, 0x00}, // }
    {0x10, 0x08, 0x08, 0x10, 0x08}, // ~
};

static void SH1106_Write_Command(uint8_t command);
static void SH1106_Write_Data(uint8_t *data, uint16_t size);
static void SH1106_Write_Letter(uint8_t index, uint8_t x, uint8_t y, SH1106_PixelState state);

/**
  * @brief Sends a command to the display via I2C
  *
  * @param command: command to be sent
  * @retval None
  */
static void SH1106_Write_Command(uint8_t command)
{
    // 0x00 -> sending a command
    HAL_I2C_Mem_Write(&hi2c1, SH1106_I2C_ADDR, SH1106_COMMAND_ADDR, 1,
        &command, 1, HAL_MAX_DELAY);
}

/**
  * @brief Sends I2C data to the display
  *
  * @param data: the address of the first byte
  * @param size: the number of bytes from data
  * @retval None
  */
static void SH1106_Write_Data(uint8_t *data, uint16_t size)
{
    // 0x40 -> sending data to GDDRAM
    HAL_I2C_Mem_Write(&hi2c1, SH1106_I2C_ADDR, SH1106_DATA_ADDR, 1,
        data, size, HAL_MAX_DELAY);
}

/**
  * @brief Writes a letter from a font map into the display.
  *
  * @param index: the position of the letter in the map
  * @param x: column [0, 127]
  * @param y: row [0, 31]
  * @retval None
  */
static void SH1106_Write_Letter(uint8_t index, uint8_t x, uint8_t y, SH1106_PixelState state) 
{
    for (uint8_t i = 0; i < 5; i++) {
        uint8_t column = SH1106_Font5x8[index][i];
        // the letter is drawn column by column
        for (uint8_t j = 0; j < 8; j++) {
            if ((column >> j) & 0x01) {
                SH1106_Draw_Pixel(x + i, y + j, state);
            }
        }
    }
}

/**
  * @brief Initializes the display sending the appropiate I2C commands
  *
  * @param None
  * @retval None
  */
void SH1106_Init(void)
{
    HAL_Delay(100);

    SH1106_Write_Command(0xAE); // Display OFF

    SH1106_Write_Command(0xD5); // Display Clock Divide Ratio
    SH1106_Write_Command(0x50); // \_ Default

    SH1106_Write_Command(0xA8); // Multiplex Ratio
    SH1106_Write_Command(0x3F); // \_ 64 rows to scan

    SH1106_Write_Command(0xD3); // Display Offset
    SH1106_Write_Command(0x00); // \_ No offset

    SH1106_Write_Command(0x40); // Display Start Line: 0

    SH1106_Write_Command(0xAD); // DC-DC Pump
    SH1106_Write_Command(0x8B); // \_ Enable charge pump

    SH1106_Write_Command(0xA1); // Mirror H
    SH1106_Write_Command(0xC8); // Mirror V

    SH1106_Write_Command(0xDA); // COM Pins Hardware Config
    SH1106_Write_Command(0x12); // 0x12 -> sequential and disable remap

    SH1106_Write_Command(0x81); // Contrast Control
    SH1106_Write_Command(0xBF); // \_ Medium-High brightness

    SH1106_Write_Command(0xD9); // Pre-charge Period
    SH1106_Write_Command(0x22); // \_ 0x22

    SH1106_Write_Command(0xDB); // VCOMH Deselect Level
    SH1106_Write_Command(0x35); // \_ 0x35

    SH1106_Write_Command(0xA4); // Resume content from GDDRAM
    SH1106_Write_Command(0xA6); // Set entire display ON

    SH1106_Clear();
    SH1106_Update_Screen();

    SH1106_Write_Command(0xAF); // Display ON
}

/**
  * @brief Writes the content of the frame buffer into de GDDRAM
  *
  * @param None
  * @retval None
  */
void SH1106_Update_Screen(void) 
{
    for (uint8_t i = 0; i < 8; i++) {
        SH1106_Write_Command(0xB0 + i); // Page address
        SH1106_Write_Command(0x02); // Lower column offset
        SH1106_Write_Command(0x10); // Higher column offset

        // Dumps the content of the buffer to the display page by page 
        SH1106_Write_Data(&SH1106_Buffer[SH1106_WIDTH * i], SH1106_WIDTH);
    }
}

/**
  * @brief Clears the display
  *
  * @param None
  * @retval None
  */
void SH1106_Clear(void) 
{
    memset(SH1106_Buffer, 0x00, SH1106_BUFFER_SIZE);
}

/**
 * @brief Writes a formatted string from a font map into the display at the given coordinates.
 *
 * @param x: X
 * @param y: Y
 * @param format: the formatted text to write
 * @retval None
 */
void SH1106_Draw_Text(uint8_t x, uint8_t y, SH1106_PixelState state, const char *text)
{
    for (uint8_t i = 0; i < strlen(text); i++)
        SH1106_Write_Letter(text[i] - 32, x + (i * 6), y, state);
}

/**
  * @brief Turns on/off the selected pixel.
  *        The byte index in the buffer is calculated using:
  *        
  *        index = x + (y / 8) * 128
  *
  *        Meaning: The page number (y / 8, as each page is 8 pixels high) 
  *        is multiplied by the page width (128 bytes) to skip all previous 
  *        pages. Then, the column offset (x) is added to find the exact byte,
  *        then the exact bit is selected using mod 8.
  *
  * @param x: column [0, 127]
  * @param y: row [0, 31]
  * @param state: PIXEL_ON (0x01) or PIXEL_OFF (0x00)
  * @retval None
  */
void SH1106_Draw_Pixel(uint8_t x, uint8_t y, SH1106_PixelState state) 
{
    if (x >= SH1106_WIDTH || y >= SH1106_HEIGHT) return; // out of range

    if (state == PIXEL_ON)
        SH1106_Buffer[x + (y / 8) * SH1106_WIDTH] |= (1 << (y % 8));
    else
        SH1106_Buffer[x + (y / 8) * SH1106_WIDTH] &= ~(1 << (y % 8));
}

/**
  * @brief Draws a monochrome bitmap on the screen
  *
  * @param x: starting column [0, 127]
  * @param y: starting row [0, 31]
  * @param bitmap: pointer to the image array
  * @param width: image width in pixels
  * @param height: image height in pixels
  * @retval None
  */
void SH1106_Draw_Bitmap(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t width, uint8_t height)
{
    for (uint8_t i = 0; i < width; i++) {
        for (uint8_t j = 0; j < height; j++) {
            // if the bit corresponding to the actual pixel is ON, it's drawn
            if (bitmap[i + (j / 8) * width] & (1 << (j % 8))) {
                SH1106_Draw_Pixel(x + i, y + j, PIXEL_ON);
            }
        }
    }
}

/**
 * @brief Draws a line wether it is X/Y only or it has
 *        a slope, using the Bresenham algorithm
 * @param x1: X1
 * @param y1: Y1
 * @param x2: X2
 * @param y2: Y2
 * @param state: PIXEL_ON/PIXEL_OFF
 */
void SH1106_Draw_Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SH1106_PixelState state)
{
    // x-y diffs
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    
    // left->right or right->left
    // up->down or down->up
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    
    // error accumulator
    int err = dx - dy;

    // init local vars
    int x = x1;
    int y = y1;

    while (1) {
        SH1106_Draw_Pixel(x, y, state);

        // if the destination is reached
        if (x == x2 && y == y2) break;

        // temporary error variable
        int e2 = 2 * err;

        // check if we need to step along the X axis
        if (e2 > -dy) {
            err -= dy; // adjust error based on the Y-axis distance
            x += sx;   // move x in the determined direction (left or right)
        }
        
        // check if we need to step along the Y axis
        if (e2 < dx) {
            err += dx; // adjust error based on the X-axis distance
            y += sy;   // move y in the determined direction (up or down)
        }
    }
}

/**
 * @brief Draws a rectangle of a given dimensions from (x, y)
 *
 * @param x: X start
 * @param y: Y start
 * @param width: X + width
 * @param height: Y + width
 * @param state: PIXEL_ON/PIXEL_OFF
 */
void SH1106_Draw_Rectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, SH1106_PixelState state)
{
    if (width == 0 || height == 0) return;

    uint8_t x_end = x + width - 1;
    uint8_t y_end = y + height - 1;

    for (uint8_t i = x; i <= x_end; i++) {
        SH1106_Draw_Pixel(i, y, state);
        SH1106_Draw_Pixel(i, y_end, state);
    }

    for (uint8_t j = y + 1; j < y_end; j++) {
        SH1106_Draw_Pixel(x, j, state);
        SH1106_Draw_Pixel(x_end, j, state);
    }
}
