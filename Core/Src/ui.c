#include "ui.h"
#include "sh1106.h"
#include "font_10x16.h"
#include "font_5x8.h"
#include "icons.h"

static TaskHandle_t uiTaskHandle;

const SH1106_Font_t Font_10x16 = {
  .height = 16,
  .width = 10,
  .map = __10x16,
  .size = SH1106_FONT_10x16
};

const SH1106_Font_t Font_5x8 = {
  .height = 8,
  .width = 5,
  .map = __5x8,
  .size = SH1106_FONT_5x8
};

void UI_Handler(void *pArgs)
{
    uint32_t tick = osKernelSysTick();

    UI_Container_t root_screen = {0, 0, 128, 64, 2};
    UI_Container_t status_bar_view, main_view;

    for(;;) {
        SH1106_Clear();

        UI_Container_Split_V(&root_screen, &status_bar_view, &main_view, 18);
        
        UI_Draw_Status_Bar(&status_bar_view, "Croppy v1.0", true);

        UI_Container_Draw_Text(&main_view, 0, 2, SH1106_PIXEL_ON, &Font_10x16, "Hum: %d%%", 100);
        UI_Container_Draw_Text(&main_view, 0, 18, SH1106_PIXEL_ON, &Font_10x16, "pH:  %d", 14);
        UI_Container_Draw_Text(&main_view, 0, 34, SH1106_PIXEL_ON, &Font_10x16, "NPK: %d%%", 100);

        SH1106_Update_Screen();

        tick += 66; // 15 FPS
        osDelayUntil(tick);
    }
}

void UI_Create_Task(void) 
{
    xTaskCreate(
        UI_Handler,
        "UI Task",
        512,
        NULL,
        2,
        &uiTaskHandle
    );
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

void UI_Container_Draw_Text(const UI_Container_t *c, uint8_t rel_x, uint8_t rel_y, SH1106_Pixel_State_t state, const SH1106_Font_t *font, const char *format, ...) 
{
    char buffer[32];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    uint8_t abs_x = c->x + c->padding + rel_x;
    uint8_t abs_y = c->y + c->padding + rel_y;

    if ((abs_x < (c->x + c->w)) && (abs_y < (c->y + c->h))) {
        SH1106_Draw_Text(abs_x, abs_y, state, font, buffer);
    }
}

void UI_Container_Draw_Button(const UI_Container_t *c, uint8_t rel_x, uint8_t rel_y, uint8_t width, uint8_t height, const char* label, bool is_focused)
{
    uint8_t abs_x = c->x + c->padding + rel_x;
    uint8_t abs_y = c->y + c->padding + rel_y;
    
    uint8_t text_width = strlen(label) * 11;
    uint8_t text_x = abs_x + ((width - text_width) / 2);
    uint8_t text_y = abs_y + ((height - 16) / 2);

    if (is_focused) {
        for (uint8_t i = abs_y; i < abs_y + height; i++) {
            for (uint8_t j = abs_x; j < abs_x + width; j++) {
                SH1106_Draw_Pixel(j, i, SH1106_PIXEL_ON);
            }
        }
        SH1106_Draw_Text(text_x, text_y, SH1106_PIXEL_OFF, &Font_10x16, label);
    } else {
        SH1106_Draw_Rectangle(abs_x, abs_y, width, height, SH1106_PIXEL_ON);
        SH1106_Draw_Text(text_x, text_y, SH1106_PIXEL_ON, &Font_10x16, label);
    }
}

void UI_Container_Draw_Icon(const UI_Container_t *c, uint8_t rel_x, uint8_t rel_y, UI_IconIndex_t icon, SH1106_Pixel_State_t state)
{
    uint8_t abs_x = c->x + c->padding + rel_x;
    uint8_t abs_y = c->y + c->padding + rel_y;

    if ((abs_x + 8 <= c->x + c->w) && (abs_y + 8 <= c->y + c->h)) {
        SH1106_Draw_Bitmap(abs_x, abs_y, UI_Icons15x15[icon], 15, 15, state);
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

    UI_Container_Draw_Text(c, 0, 1, SH1106_PIXEL_OFF, &Font_5x8, title);

    if (battery_low) {
        uint8_t icon_rel_x = c->w - 8 - (c->padding * 2);
        UI_Container_Draw_Icon(c, icon_rel_x, 1, ICON_BATTERY_LOW, SH1106_PIXEL_OFF);
    }
}