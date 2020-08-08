// game objects header
#ifndef GAME_OBJECTS_H
#define GAME_OBJECTS_H

#include <cstdio>
#include <cstdint>

// programming CPU is easier than GPU
// will render on CPU using a buffer then pass to GPU to draw pixels
typedef struct Buffer_Struct
{
    size_t width, height;
    // to store 4 8-bit pixel color values
    uint32_t* data;
} Buffer;

typedef struct Sprite_Struct
{
    size_t width, height;
    uint8_t* data;
} Sprite;

bool sprite_overlap_check(
    const Sprite& sp_a, size_t x_a, size_t y_a,
    const Sprite& sp_b, size_t x_b, size_t y_b);

void buffer_draw_sprite(Buffer* buffer, const Sprite& sprite, size_t x, size_t y, uint32_t color);

#endif