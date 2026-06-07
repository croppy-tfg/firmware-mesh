#include "ui.h"

static TaskHandle_t uiTaskHandle;
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

    UI_Container_t main_view = {0, 10, 128, 54, 2}; 
    UI_Container_t left_col, right_col;

    for(;;) {
        SH1106_Clear();

        UI_Draw_Status_Bar("Croppy v1.0", false);

        UI_Container_Split_H(&main_view, &left_col, &right_col, 65);

        UI_Container_Draw_Text(&left_col, 0, 0,  SH1106_PIXEL_ON, "Hum: %d%%", 78);
        UI_Container_Draw_Text(&left_col, 0, 12, SH1106_PIXEL_ON, "Tmp: %dC", 24);
        UI_Container_Draw_Text(&left_col, 0, 24, SH1106_PIXEL_ON, "pH:  %d", 7);

        UI_Container_Draw_Button(&right_col, 2, 12, 40, 20, "MENU", true);

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

void UI_Draw_Status_Bar(const char *title, bool battery_low)
{
    for (uint8_t y = 0; y < 10; y++) {
        for (uint8_t x = 0; x < 128; x++) {
            SH1106_Draw_Pixel(x, y, SH1106_PIXEL_ON);
        }
    }
    SH1106_Draw_Text(2, 1, SH1106_PIXEL_OFF, title);

    if (battery_low) {
        SH1106_Draw_Text(109, 1, SH1106_PIXEL_OFF, "BAT");
    }
}