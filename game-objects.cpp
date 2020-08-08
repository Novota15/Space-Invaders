#include "game-objects.h"

// clear buffer to a certain color
void buffer_clear(Buffer* buffer, uint32_t color) {
    for (size_t i = 0; i < buffer->width * buffer->height; ++i) {
        buffer->data[i] = color;
    }
}

bool sprite_overlap_check(
    const Sprite& sp_a, size_t x_a, size_t y_a,
    const Sprite& sp_b, size_t x_b, size_t y_b) {
    // if the rectangles overlap, we should
    // further check if any pixel of sprite A overlap with any of
    // sprite B
    if (x_a < x_b + sp_b.width && x_a + sp_a.width > x_b &&
       y_a < y_b + sp_b.height && y_a + sp_a.height > y_b) {
        return true;
    }
    return false;
}

void buffer_draw_sprite(Buffer* buffer, const Sprite& sprite, size_t x, size_t y, uint32_t color) {
    for (size_t xi = 0; xi < sprite.width; ++xi) {
        for (size_t yi = 0; yi < sprite.height; ++yi) {
            if (sprite.data[yi * sprite.width + xi] &&
               (sprite.height - 1 + y - yi) < buffer->height &&
               (x + xi) < buffer->width) {
                buffer->data[(sprite.height - 1 + y - yi) * buffer->width + (x + xi)] = color;
            }
        }
    }
}

// assumes array of size 6 is passed
void build_alien_sprites(Sprite *alien_sprites) {
    alien_sprites[0].width = 8;
    alien_sprites[0].height = 8;
    alien_sprites[0].data = new uint8_t[64]
    {
        0,0,0,1,1,0,0,0, // . . . @ @ . . .
        0,0,1,1,1,1,0,0, // . . @ @ @ @ . .
        0,1,1,1,1,1,1,0, // . @ @ @ @ @ @ .
        1,1,0,1,1,0,1,1, // @ @ . @ @ . @ @
        1,1,1,1,1,1,1,1, // @ @ @ @ @ @ @ @
        0,1,0,1,1,0,1,0, // . @ . @ @ . @ .
        1,0,0,0,0,0,0,1, // @ . . . . . . @
        0,1,0,0,0,0,1,0  // . @ . . . . @ .
    };

    alien_sprites[1].width = 8;
    alien_sprites[1].height = 8;
    alien_sprites[1].data = new uint8_t[64]
    {
        0,0,0,1,1,0,0,0, // . . . @ @ . . .
        0,0,1,1,1,1,0,0, // . . @ @ @ @ . .
        0,1,1,1,1,1,1,0, // . @ @ @ @ @ @ .
        1,1,0,1,1,0,1,1, // @ @ . @ @ . @ @
        1,1,1,1,1,1,1,1, // @ @ @ @ @ @ @ @
        0,0,1,0,0,1,0,0, // . . @ . . @ . .
        0,1,0,1,1,0,1,0, // . @ . @ @ . @ .
        1,0,1,0,0,1,0,1  // @ . @ . . @ . @
    };

    alien_sprites[2].width = 11;
    alien_sprites[2].height = 8;
    alien_sprites[2].data = new uint8_t[88]
    {
        0,0,1,0,0,0,0,0,1,0,0, // . . @ . . . . . @ . .
        0,0,0,1,0,0,0,1,0,0,0, // . . . @ . . . @ . . .
        0,0,1,1,1,1,1,1,1,0,0, // . . @ @ @ @ @ @ @ . .
        0,1,1,0,1,1,1,0,1,1,0, // . @ @ . @ @ @ . @ @ .
        1,1,1,1,1,1,1,1,1,1,1, // @ @ @ @ @ @ @ @ @ @ @
        1,0,1,1,1,1,1,1,1,0,1, // @ . @ @ @ @ @ @ @ . @
        1,0,1,0,0,0,0,0,1,0,1, // @ . @ . . . . . @ . @
        0,0,0,1,1,0,1,1,0,0,0  // . . . @ @ . @ @ . . .
    };

    alien_sprites[3].width = 11;
    alien_sprites[3].height = 8;
    alien_sprites[3].data = new uint8_t[88]
    {
        0,0,1,0,0,0,0,0,1,0,0, // . . @ . . . . . @ . .
        1,0,0,1,0,0,0,1,0,0,1, // @ . . @ . . . @ . . @
        1,0,1,1,1,1,1,1,1,0,1, // @ . @ @ @ @ @ @ @ . @
        1,1,1,0,1,1,1,0,1,1,1, // @ @ @ . @ @ @ . @ @ @
        1,1,1,1,1,1,1,1,1,1,1, // @ @ @ @ @ @ @ @ @ @ @
        0,1,1,1,1,1,1,1,1,1,0, // . @ @ @ @ @ @ @ @ @ .
        0,0,1,0,0,0,0,0,1,0,0, // . . @ . . . . . @ . .
        0,1,0,0,0,0,0,0,0,1,0  // . @ . . . . . . . @ .
    };

    alien_sprites[4].width = 12;
    alien_sprites[4].height = 8;
    alien_sprites[4].data = new uint8_t[96]
    {
        0,0,0,0,1,1,1,1,0,0,0,0, // . . . . @ @ @ @ . . . .
        0,1,1,1,1,1,1,1,1,1,1,0, // . @ @ @ @ @ @ @ @ @ @ .
        1,1,1,1,1,1,1,1,1,1,1,1, // @ @ @ @ @ @ @ @ @ @ @ @
        1,1,1,0,0,1,1,0,0,1,1,1, // @ @ @ . . @ @ . . @ @ @
        1,1,1,1,1,1,1,1,1,1,1,1, // @ @ @ @ @ @ @ @ @ @ @ @
        0,0,0,1,1,0,0,1,1,0,0,0, // . . . @ @ . . @ @ . . .
        0,0,1,1,0,1,1,0,1,1,0,0, // . . @ @ . @ @ . @ @ . .
        1,1,0,0,0,0,0,0,0,0,1,1  // @ @ . . . . . . . . @ @
    };


    alien_sprites[5].width = 12;
    alien_sprites[5].height = 8;
    alien_sprites[5].data = new uint8_t[96]
    {
        0,0,0,0,1,1,1,1,0,0,0,0, // . . . . @ @ @ @ . . . .
        0,1,1,1,1,1,1,1,1,1,1,0, // . @ @ @ @ @ @ @ @ @ @ .
        1,1,1,1,1,1,1,1,1,1,1,1, // @ @ @ @ @ @ @ @ @ @ @ @
        1,1,1,0,0,1,1,0,0,1,1,1, // @ @ @ . . @ @ . . @ @ @
        1,1,1,1,1,1,1,1,1,1,1,1, // @ @ @ @ @ @ @ @ @ @ @ @
        0,0,1,1,1,0,0,1,1,1,0,0, // . . @ @ @ . . @ @ @ . .
        0,1,1,0,0,1,1,0,0,1,1,0, // . @ @ . . @ @ . . @ @ .
        0,0,1,1,0,0,0,0,1,1,0,0  // . . @ @ . . . . @ @ . .
    };
    return;
}

Sprite build_alien_death_sprite() {
    Sprite alien_death_sprite;
    alien_death_sprite.width = 13;
    alien_death_sprite.height = 7;
    alien_death_sprite.data = new uint8_t[91]
    {
        0,1,0,0,1,0,0,0,1,0,0,1,0, // . @ . . @ . . . @ . . @ .
        0,0,1,0,0,1,0,1,0,0,1,0,0, // . . @ . . @ . @ . . @ . .
        0,0,0,1,0,0,0,0,0,1,0,0,0, // . . . @ . . . . . @ . . .
        1,1,0,0,0,0,0,0,0,0,0,1,1, // @ @ . . . . . . . . . @ @
        0,0,0,1,0,0,0,0,0,1,0,0,0, // . . . @ . . . . . @ . . .
        0,0,1,0,0,1,0,1,0,0,1,0,0, // . . @ . . @ . @ . . @ . .
        0,1,0,0,1,0,0,0,1,0,0,1,0  // . @ . . @ . . . @ . . @ .
    };
    return alien_death_sprite;
}

Sprite build_player_sprite() {
    Sprite player_sprite;
    player_sprite.width = 11;
    player_sprite.height = 7;
    player_sprite.data = new uint8_t[77]
    {
        0,0,0,0,0,1,0,0,0,0,0, // . . . . . @ . . . . .
        0,0,0,0,1,1,1,0,0,0,0, // . . . . @ @ @ . . . .
        0,0,0,0,1,1,1,0,0,0,0, // . . . . @ @ @ . . . .
        0,1,1,1,1,1,1,1,1,1,0, // . @ @ @ @ @ @ @ @ @ .
        1,1,1,1,1,1,1,1,1,1,1, // @ @ @ @ @ @ @ @ @ @ @
        1,1,1,1,1,1,1,1,1,1,1, // @ @ @ @ @ @ @ @ @ @ @
        1,1,1,1,1,1,1,1,1,1,1, // @ @ @ @ @ @ @ @ @ @ @
    };
    return player_sprite;
}

Sprite build_bullet_sprite() {
    Sprite bullet_sprite;
    bullet_sprite.width = 1;
    bullet_sprite.height = 3;
    bullet_sprite.data = new uint8_t[3]
    {
        1, // @
        1, // @
        1  // @
    };
    return bullet_sprite;
}

void build_alien_animation(Sprite *alien_sprites, Sprite_Animation *alien_animation) {
    for (size_t i = 0; i < 3; ++i) {
        alien_animation[i].loop = true;
        alien_animation[i].num_frames = 2;
        alien_animation[i].frame_duration = 10;
        alien_animation[i].time = 0;

        alien_animation[i].frames = new Sprite*[2];
        alien_animation[i].frames[0] = &alien_sprites[2 * i];
        alien_animation[i].frames[1] = &alien_sprites[2 * i + 1];
    }
    return;
}