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

typedef struct Alien_Struct
{
    size_t x, y;
    uint8_t type;
} Alien;

typedef struct Player_Struct
{
    size_t x, y;
    size_t life;
} Player;

typedef struct Bullet_Struct
{
    size_t x, y;
    int dir;
} Bullet;

typedef struct Game_Struct
{
    size_t width, height;
    size_t num_aliens;
    Alien* aliens;
    Player player;
    Bullet bullets[GAME_MAX_BULLETS];
} Game;

typedef struct Sprite_Animation_Struct
{
    bool loop;
    size_t num_frames;
    size_t frame_duration;
    size_t time;
    Sprite** frames;
} Sprite_Animation;

void buffer_clear(Buffer* buffer, uint32_t color);

bool sprite_overlap_check(
    const Sprite& sp_a, size_t x_a, size_t y_a,
    const Sprite& sp_b, size_t x_b, size_t y_b);

void buffer_draw_sprite(Buffer* buffer, const Sprite& sprite, size_t x, size_t y, uint32_t color);

void build_alien_sprites(Sprite *alien_sprites);

Sprite build_alien_death_sprite();

Sprite build_player_sprite();

#endif