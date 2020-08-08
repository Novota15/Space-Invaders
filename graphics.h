// game graphics header
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "main.h"

void buffer_draw_number(Buffer* buffer, const Sprite& number_spritesheet, size_t number, size_t x, size_t y, uint32_t color);

void buffer_draw_text(Buffer* buffer, const Sprite& text_spritesheet,const char* text, size_t x, size_t y, uint32_t color);

#endif