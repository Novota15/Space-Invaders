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

#endif