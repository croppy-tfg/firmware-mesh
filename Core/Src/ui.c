#include "ui.h"
#include "sh1106.h"

static TaskHandle_t uiTaskHandle;

static const uint8_t UI_Icons8x8[][8] = {
    // [0] empty battery
    {0x7C, 0x44, 0x44, 0x44, 0x44, 0x44, 0x7C, 0x10},
    // [1] low battery
    {0x7C, 0x7C, 0x44, 0x44, 0x44, 0x44, 0x7C, 0x10},
    // [2] mid battery
    {0x7C, 0x7C, 0x7C, 0x7C, 0x44, 0x44, 0x7C, 0x10},
    // [3] full battery
    {0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x10},
    // [4] water drop
    {0x10, 0x28, 0x48, 0x84, 0x84, 0x48, 0x30, 0x00},
    // [5] thermometer
    {0x1C, 0x22, 0x2A, 0x2A, 0x22, 0x42, 0x3C, 0x00},
    // [6] flask
    {0x1C, 0x08, 0x08, 0x14, 0x22, 0x41, 0x7F, 0x00},
    // [7] network connected
    {0x03, 0x0F, 0x33, 0xCC, 0xCC, 0x33, 0x0F, 0x03},
    // [8] network disconnected
    {0x03, 0x8F, 0xBB, 0xEC, 0xEC, 0xBB, 0x8F, 0x03},
    // [9] pump/motor
    {0x04, 0x14, 0x24, 0xFF, 0x24, 0x28, 0x20, 0x00}
};

void UI_Handler(void *pArgs);

void UI_Create_Task(void) 
{
    xTaskCreate(
        UI_Handler,
        "UI Task",
        256,
        NULL,
        2,
        &uiTaskHandle
    );
}

void UI_Handler(void *pArgs)
{
    uint32_t tick = osKernelSysTick();

    UI_Container_t root_screen = {0, 0, 128, 64, 2};
    UI_Container_t status_bar_view, main_view;

    for(;;) {
        SH1106_Clear();

        UI_Container_Split_V(&root_screen, &status_bar_view, &main_view, 18);
        
        UI_Draw_Status_Bar(&status_bar_view, "Croppy v1.0", true);

        UI_Container_Draw_Icon(&main_view, 0, 0, ICON_WATER_DROP, SH1106_PIXEL_ON);
        UI_Container_Draw_Text(&main_view, 12, 0, SH1106_PIXEL_ON, "Hum: %d%%", 45);
            
        UI_Container_Draw_Icon(&main_view, 0, 14, ICON_FLASK, SH1106_PIXEL_ON);
        UI_Container_Draw_Text(&main_view, 12, 14, SH1106_PIXEL_ON, "pH:  %d", 7);

        SH1106_Update_Screen();

        tick += 66; // 15 FPS
        osDelayUntil(tick);
    }
}

void UI_Container_Split_H(const UI_Container_t *parent, UI_Container_t *left, UI_Container_t *right, uint8_t split_pct) 
{
    uint8_t left_w = (parent->w * split_pct) / 100;
    
    *left = (UI_Container_t) {
        parent->x,
        parent->y,
        left_w,
        parent->h,
        parent->padding
    };

    *right = (UI_Container_t) {
        parent->x + left_w,
        parent->y,
        parent->w - left_w,
        parent->h,
        parent->padding
    };
}

void UI_Container_Split_V(const UI_Container_t *parent, UI_Container_t *top, UI_Container_t *bottom, uint8_t split_pct) 
{
    uint8_t top_h = (parent->h * split_pct) / 100;
    
    *top = (UI_Container_t) {
        parent->x,
        parent->y,
        parent->w,
        top_h,
        parent->padding
    };
    
    *bottom = (UI_Container_t) {
        parent->x,
        parent->y + top_h,
        parent->w,
        parent->h - top_h,
        parent->padding
    };
}

void UI_Container_Draw_Text(const UI_Container_t *c, uint8_t rel_x, uint8_t rel_y, SH1106_Pixel_State_t state, const char *format, ...) 
{
    char buffer[32];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    uint8_t abs_x = c->x + c->padding + rel_x;
    uint8_t abs_y = c->y + c->padding + rel_y;

    if ((abs_x < (c->x + c->w)) && (abs_y < (c->y + c->h))) {
        SH1106_Draw_Text(abs_x, abs_y, state, buffer);
    }
}

void UI_Container_Draw_Button(const UI_Container_t *c, uint8_t rel_x, uint8_t rel_y, uint8_t width, uint8_t height, const char* label, bool is_focused)
{
    uint8_t abs_x = c->x + c->padding + rel_x;
    uint8_t abs_y = c->y + c->padding + rel_y;
    
    uint8_t text_width = strlen(label) * 6;
    uint8_t text_x = abs_x + ((width - text_width) / 2);
    uint8_t text_y = abs_y + ((height - 8) / 2);

    if (is_focused) {
        for (uint8_t i = abs_y; i < abs_y + height; i++) {
            for (uint8_t j = abs_x; j < abs_x + width; j++) {
                SH1106_Draw_Pixel(j, i, SH1106_PIXEL_ON);
            }
        }
        SH1106_Draw_Text(text_x, text_y, SH1106_PIXEL_OFF, label);
    } else {
        SH1106_Draw_Rectangle(abs_x, abs_y, width, height, SH1106_PIXEL_ON);
        SH1106_Draw_Text(text_x, text_y, SH1106_PIXEL_ON, label);
    }
}

void UI_Container_Draw_Icon(const UI_Container_t *c, uint8_t rel_x, uint8_t rel_y, UI_IconIndex_t icon, SH1106_Pixel_State_t state)
{
    uint8_t abs_x = c->x + c->padding + rel_x;
    uint8_t abs_y = c->y + c->padding + rel_y;

    if ((abs_x + 8 <= c->x + c->w) && (abs_y + 8 <= c->y + c->h)) {
        SH1106_Draw_Bitmap(abs_x, abs_y, UI_Icons8x8[icon], 8, 8, state);
    }
}

void UI_Draw_Status_Bar(const UI_Container_t *c, const char *title, bool battery_low)
{
    if (c == NULL) return;

    for (uint8_t y = c->y; y < (c->y + c->h); y++) {
        for (uint8_t x = c->x; x < (c->x + c->w); x++) {
            SH1106_Draw_Pixel(x, y, SH1106_PIXEL_ON);
        }
    }

    UI_Container_Draw_Text(c, 0, 1, SH1106_PIXEL_OFF, title);

    if (battery_low) {
        uint8_t icon_rel_x = c->w - 8 - (c->padding * 2);
        UI_Container_Draw_Icon(c, icon_rel_x, 1, ICON_BATTERY_LOW, SH1106_PIXEL_OFF);
    }
}