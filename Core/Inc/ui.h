/**
  * @file    ui.h
  * @author  JM
  * @brief   App layer functions for UI management
  * @date    2026
  */

#ifndef __UI_H__
#define __UI_H__

#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "sh1106.h"

typedef struct {
  uint8_t x;
  uint8_t y;
  uint8_t w;
  uint8_t h;
  uint8_t padding;
} UI_Container_t;

void UI_Create_Task(void);

void UI_Container_Split_H(const UI_Container_t *parent, UI_Container_t *left, UI_Container_t *right, uint8_t split_pct);
void UI_Container_Split_V(const UI_Container_t *parent, UI_Container_t *top, UI_Container_t *bottom, uint8_t split_pct);

void UI_Container_Draw_Text(const UI_Container_t *c, uint8_t rel_x, uint8_t rel_y, SH1106_Pixel_State_t state, const char *format, ...);
void UI_Container_Draw_Button(const UI_Container_t *c, uint8_t rel_x, uint8_t rel_y, uint8_t width, uint8_t height, const char* label, bool is_focused);

void UI_Draw_Status_Bar(const char *title, bool battery_low);
void UI_Draw_Splash_Screen(void);

#endif // __UI_H__