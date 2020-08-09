#include "main.h"
#include "validators.h"
#include "graphics.h"
#include "game-objects.h"
#include "controls.h"

// global vars
int move_dir = 0;
bool fire_pressed = 0;
bool game_running = false;

// error and debug
#define GL_ERROR_CASE(glerror)\
    case glerror: snprintf(error, sizeof(error), "%s", #glerror)

inline void gl_debug(const char *file, int line) {
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR){
        char error[128];

        switch(err) {
            GL_ERROR_CASE(GL_INVALID_ENUM); break;
            GL_ERROR_CASE(GL_INVALID_VALUE); break;
            GL_ERROR_CASE(GL_INVALID_OPERATION); break;
            GL_ERROR_CASE(GL_INVALID_FRAMEBUFFER_OPERATION); break;
            GL_ERROR_CASE(GL_OUT_OF_MEMORY); break;
            default: snprintf(error, sizeof(error), "%s", "UNKNOWN_ERROR"); break;
        }

        fprintf(stderr, "%s - %s: %d\n", error, file, line);
    }
}

#undef GL_ERROR_CASE

// error events reported through callbacks
void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

uint32_t xorshift32(uint32_t* rng) {
    uint32_t x = *rng;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *rng = x;
    return x;
}

double random(uint32_t* rng) {
    return (double)xorshift32(rng) / std::numeric_limits<uint32_t>::max();
}

int initialize_glfw() {
    // initialize the GLFW library
    if (!glfwInit()) return -1;

    // give GLFW the appropriate hints before creating window
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    return 0;
}

GLFWwindow* create_game_window(size_t buffer_width, size_t buffer_height) {
    GLFWwindow* window = glfwCreateWindow(2 * buffer_width, 2 * buffer_height, "Space Invaders", NULL, NULL);
    if (!window) {
        glfwTerminate();
        // return -1;
    }
    return window;
}

int main(int argc, char* argv[]) {
    // error reporting
    glfwSetErrorCallback(error_callback);

    if (initialize_glfw() == -1) return -1;

    // initialize the GLFW library
    // if (!glfwInit()) return -1;

    // give GLFW the appropriate hints before creating window
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // create game window
    const size_t buffer_width = 224;
    const size_t buffer_height = 256;
    GLFWwindow* window = create_game_window(buffer_width, buffer_height);

    // arrow key functionality
    glfwSetKeyCallback(window, key_callback);

    // tell GFLW to make subsequent OpenGL calls apply to current 
    glfwMakeContextCurrent(window);

    // initialize GLEW
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error initializing GLEW.\n");
        glfwTerminate();
        return -1;
    }

    // can make calls to opengl functions now
    // query the opengl version first
    int glVersion[2] = {-1,1};
    glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]);
    glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]);

    gl_debug(__FILE__, __LINE__);

    // report versions used
    printf("Playing Space Invaders, created by Grant Novota\n");
    printf("Using OpenGL: %d.%d\n", glVersion[0], glVersion[1]);
    printf("Renderer used: %s\n", glGetString(GL_RENDERER));
    printf("Shading Language: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    glClearColor(1.0, 0.0, 0.0, 1.0);

    Buffer buffer;
    buffer.width = buffer_width;
    buffer.height = buffer_height;
    buffer.data   = new uint32_t[buffer.width * buffer.height];
    buffer_clear(&buffer, 0);

    // create textures for giving buffer to opengl
    GLuint buffer_texture;
    glGenTextures(1, &buffer_texture);
    glBindTexture(GL_TEXTURE_2D, buffer_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, buffer.width, buffer.height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, buffer.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // create vao for making fullscreen triangle
    GLuint fullscreen_triangle_vao;
    glGenVertexArrays(1, &fullscreen_triangle_vao);

    // create shader for displaying buffer
    static const char* fragment_shader =
        "\n"
        "#version 330\n"
        "\n"
        "uniform sampler2D buffer;\n"
        "noperspective in vec2 TexCoord;\n"
        "\n"
        "out vec3 outColor;\n"
        "\n"
        "void main(void){\n"
        "    outColor = texture(buffer, TexCoord).rgb;\n"
        "}\n";

    static const char* vertex_shader =
        "\n"
        "#version 330\n"
        "\n"
        "noperspective out vec2 TexCoord;\n"
        "\n"
        "void main(void){\n"
        "\n"
        "    TexCoord.x = (gl_VertexID == 2)? 2.0: 0.0;\n"
        "    TexCoord.y = (gl_VertexID == 1)? 2.0: 0.0;\n"
        "    \n"
        "    gl_Position = vec4(2.0 * TexCoord - 1.0, 0.0, 1.0);\n"
        "}\n";
    
    GLuint shader_id = glCreateProgram();

    // create vertex shader
    {
        GLuint shader_vp = glCreateShader(GL_VERTEX_SHADER);

        glShaderSource(shader_vp, 1, &vertex_shader, 0);
        glCompileShader(shader_vp);
        validate_shader(shader_vp, vertex_shader);
        glAttachShader(shader_id, shader_vp);

        glDeleteShader(shader_vp);
    }

    // create fragment shader
    {
        GLuint shader_fp = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(shader_fp, 1, &fragment_shader, 0);
        glCompileShader(shader_fp);
        validate_shader(shader_fp, fragment_shader);
        glAttachShader(shader_id, shader_fp);

        glDeleteShader(shader_fp);
    }

    glLinkProgram(shader_id);

    if (!validate_program(shader_id)) {
        fprintf(stderr, "Error while validating shader.\n");
        glfwTerminate();
        glDeleteVertexArrays(1, &fullscreen_triangle_vao);
        delete[] buffer.data;
        return -1;
    }

    glUseProgram(shader_id);

    GLint location = glGetUniformLocation(shader_id, "buffer");
    glUniform1i(location, 0);

    // opengl setup
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(fullscreen_triangle_vao);

    // prepare game

    // build sprites
    Sprite alien_sprites[6];
    build_alien_sprites(alien_sprites);

    Sprite alien_death_sprite = build_alien_death_sprite();
    Sprite player_sprite = build_player_sprite();
    Sprite player_bullet_sprite = build_player_bullet_sprite();

    Sprite alien_bullet_sprite[2];
    build_alien_bullet_sprites(alien_bullet_sprite);

    Sprite text_spritesheet = build_text_spritesheet();
    Sprite number_spritesheet = text_spritesheet;
    number_spritesheet.data += 16 * 35;

    size_t alien_update_frequency = 120;
    Sprite_Animation alien_animation[3];
    build_alien_animation(alien_sprites, alien_animation, alien_update_frequency);

    Sprite_Animation alien_bullet_animation = build_alien_bullet_animation(alien_bullet_sprite);

    Game game;
    game.width = buffer_width;
    game.height = buffer_height;
    game.num_bullets = 0;
    game.num_aliens = 55;
    game.aliens = new Alien[game.num_aliens];

    game.player.x = 112 - 5;
    game.player.y = 32;

    game.player.life = 3;

    size_t alien_swarm_position = 24;
    size_t alien_swarm_max_position = game.width - 16 * 11 - 3;

    size_t aliens_killed = 0;
    size_t alien_update_timer = 0;
    bool should_change_speed = false;

    for (size_t xi = 0; xi < 11; ++xi) {
        for (size_t yi = 0; yi < 5; ++yi) {
            Alien& alien = game.aliens[xi * 5 + yi];
            alien.type = (5 - yi) / 2 + 1;

            const Sprite& sprite = alien_sprites[2 * (alien.type - 1)];

            alien.x = 16 * xi + alien_swarm_position + (alien_death_sprite.width - sprite.width)/2;
            alien.y = 17 * yi + 128;
        }
    }

    uint8_t* death_counters = new uint8_t[game.num_aliens];
    for (size_t i = 0; i < game.num_aliens; ++i) {
        death_counters[i] = 10;
    }

    // background color
    uint32_t clear_color = rgb_to_uint32(29, 28, 60);
    uint32_t rng = 13;

    int alien_move_dir = 4;

    // game info
    size_t score = 0;
    size_t credits = 0;

    game_running = true;

    int player_move_dir = 0;

    // the game loop
    while (!glfwWindowShouldClose(window) && game_running) {
        buffer_clear(&buffer, clear_color);

        if (game.player.life == 0) {
            buffer_draw_text(&buffer, text_spritesheet, "GAME OVER", game.width / 2 - 30, game.height / 2, rgb_to_uint32(216, 218, 101));
            buffer_draw_text(&buffer, text_spritesheet, "SCORE", 4, game.height - text_spritesheet.height - 7, rgb_to_uint32(216, 218, 101));
            buffer_draw_number(&buffer, number_spritesheet, score, 4 + 2 * number_spritesheet.width, game.height - 2 * number_spritesheet.height - 12, rgb_to_uint32(216, 218, 101));

            glTexSubImage2D(
                GL_TEXTURE_2D, 0, 0, 0,
                buffer.width, buffer.height,
                GL_RGBA, GL_UNSIGNED_INT_8_8_8_8,
                buffer.data
            );
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            glfwSwapBuffers(window);
            glfwPollEvents();
            continue;
        }

        // Draw
        buffer_draw_text(&buffer, text_spritesheet, "SCORE", 4, game.height - text_spritesheet.height - 7, rgb_to_uint32(216, 218, 101));
        buffer_draw_number(&buffer, number_spritesheet, score, 4 + 2 * number_spritesheet.width, game.height - 2 * number_spritesheet.height - 12, rgb_to_uint32(216, 218, 101));

        {
            char credit_text[16];
            sprintf(credit_text, "CREDIT %02lu", credits);
            buffer_draw_text(&buffer, text_spritesheet, credit_text, 164, 7, rgb_to_uint32(216, 218, 101));
        }


        buffer_draw_number(&buffer, number_spritesheet, game.player.life, 4, 7, rgb_to_uint32(216, 218, 101));
        size_t xp =  11 + number_spritesheet.width;
        for (size_t i = 0; i < game.player.life - 1; ++i) {
            buffer_draw_sprite(&buffer, player_sprite, xp, 7, rgb_to_uint32(216, 218, 101));
            xp += player_sprite.width + 2;
        }

        for (size_t i = 0; i < game.width; ++i) {
            buffer.data[game.width * 16 + i] = rgb_to_uint32(216, 218, 101);
        }


        for (size_t ai = 0; ai < game.num_aliens; ++ai) {
            if (death_counters[ai] == 0) continue;

            const Alien& alien = game.aliens[ai];
            if (alien.type == ALIEN_DEAD) {
                buffer_draw_sprite(&buffer, alien_death_sprite, alien.x, alien.y, rgb_to_uint32(68, 225, 38));
            } else {
                const Sprite_Animation& animation = alien_animation[alien.type - 1];
                size_t current_frame = animation.time / animation.frame_duration;
                const Sprite& sprite = *animation.frames[current_frame];
                buffer_draw_sprite(&buffer, sprite, alien.x, alien.y, rgb_to_uint32(68, 225, 38));
            }
        }

        for (size_t bi = 0; bi < game.num_bullets; ++bi) {
            const Bullet& bullet = game.bullets[bi];
            const Sprite* sprite;
            if(bullet.dir > 0) sprite = &player_bullet_sprite;
            else {
                size_t cf = alien_bullet_animation.time / alien_bullet_animation.frame_duration;
                sprite = &alien_bullet_sprite[cf];
            }
            buffer_draw_sprite(&buffer, *sprite, bullet.x, bullet.y, rgb_to_uint32(255, 5, 219));
        }

        buffer_draw_sprite(&buffer, player_sprite, game.player.x, game.player.y, rgb_to_uint32(216, 218, 101));

        glTexSubImage2D(
            GL_TEXTURE_2D, 0, 0, 0,
            buffer.width, buffer.height,
            GL_RGBA, GL_UNSIGNED_INT_8_8_8_8,
            buffer.data
        );
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);

        // Simulate bullets
        for (size_t bi = 0; bi < game.num_bullets; ++bi) {
            game.bullets[bi].y += game.bullets[bi].dir;
            if(game.bullets[bi].y >= game.height || game.bullets[bi].y < player_bullet_sprite.height) {
                game.bullets[bi] = game.bullets[game.num_bullets - 1];
                --game.num_bullets;
                continue;
            }

            // Alien bullet
            if (game.bullets[bi].dir < 0) {
                bool overlap = sprite_overlap_check(
                    alien_bullet_sprite[0], game.bullets[bi].x, game.bullets[bi].y,
                    player_sprite, game.player.x, game.player.y
                );

                if (overlap) {
                    --game.player.life;
                    game.bullets[bi] = game.bullets[game.num_bullets - 1];
                    --game.num_bullets;
                    // note: the rest of the frame is still going to be simulated.
                    // maybe we need to check if the game is over or not.
                    break;
                }
            }
            // Player bullet
            else {
                // Check if player bullet hits an alien bullet
                for (size_t bj = 0; bj < game.num_bullets; ++bj) {
                    if (bi == bj) continue;

                    bool overlap = sprite_overlap_check(
                        player_bullet_sprite, game.bullets[bi].x, game.bullets[bi].y,
                        alien_bullet_sprite[0], game.bullets[bj].x, game.bullets[bj].y
                    );

                    if (overlap) {
                        // make sure it works.
                        if (bj == game.num_bullets - 1) {
                            game.bullets[bi] = game.bullets[game.num_bullets - 2];
                        } else if(bi == game.num_bullets - 1) {
                            game.bullets[bj] = game.bullets[game.num_bullets - 2];
                        } else {
                            game.bullets[(bi < bj)? bi: bj] = game.bullets[game.num_bullets - 1];
                            game.bullets[(bi < bj)? bj: bi] = game.bullets[game.num_bullets - 2];
                        }
                        game.num_bullets -= 2;
                        break;
                    }
                }

                // Check hit
                for (size_t ai = 0; ai < game.num_aliens; ++ai) {
                    const Alien& alien = game.aliens[ai];
                    if (alien.type == ALIEN_DEAD) continue;

                    const Sprite_Animation& animation = alien_animation[alien.type - 1];
                    size_t current_frame = animation.time / animation.frame_duration;
                    const Sprite& alien_sprite = *animation.frames[current_frame];
                    bool overlap = sprite_overlap_check(
                        player_bullet_sprite, game.bullets[bi].x, game.bullets[bi].y,
                        alien_sprite, alien.x, alien.y
                    );

                    if (overlap) {
                        score += 10 * (4 - game.aliens[ai].type);
                        game.aliens[ai].type = ALIEN_DEAD;
                        // NOTE: Hack to recenter death sprite
                        game.aliens[ai].x -= (alien_death_sprite.width - alien_sprite.width)/2;
                        game.bullets[bi] = game.bullets[game.num_bullets - 1];
                        --game.num_bullets;
                        ++aliens_killed;

                        if (aliens_killed % 15 == 0) should_change_speed = true;

                        break;
                    }
                }
            }
        }

        // Simulate aliens
        if (should_change_speed) {
            should_change_speed = false;
            alien_update_frequency /= 2;
            for (size_t i = 0; i < 3; ++i) {
                alien_animation[i].frame_duration = alien_update_frequency;
            }
        }

        // Update death counters
        for (size_t ai = 0; ai < game.num_aliens; ++ai) {
            const Alien& alien = game.aliens[ai];
            if (alien.type == ALIEN_DEAD && death_counters[ai]) {
                --death_counters[ai];
            }
        }

        if (alien_update_timer >= alien_update_frequency) {
            alien_update_timer = 0;

            if ((int)alien_swarm_position + alien_move_dir < 0) {
                alien_move_dir *= -1;
                //TODO: Perhaps if aliens get close enough to player, we need to check
                //for overlap. What happens when alien moves over line y = 0 line?
                for (size_t ai = 0; ai < game.num_aliens; ++ai) {
                    Alien& alien = game.aliens[ai];
                    alien.y -= 8;
                }
            } else if (alien_swarm_position > alien_swarm_max_position - alien_move_dir) {
                alien_move_dir *= -1;
            }
            alien_swarm_position += alien_move_dir;

            for (size_t ai = 0; ai < game.num_aliens; ++ai) {
                Alien& alien = game.aliens[ai];
                alien.x += alien_move_dir;
            }

            if (aliens_killed < game.num_aliens) {
                size_t rai = game.num_aliens * random(&rng);
                while(game.aliens[rai].type == ALIEN_DEAD) {
                    rai = game.num_aliens * random(&rng);
                }
                const Sprite& alien_sprite = *alien_animation[game.aliens[rai].type - 1].frames[0];
                game.bullets[game.num_bullets].x = game.aliens[rai].x + alien_sprite.width / 2;
                game.bullets[game.num_bullets].y = game.aliens[rai].y - alien_bullet_sprite[0].height;
                game.bullets[game.num_bullets].dir = -2;
                ++game.num_bullets;
            }
        }

        // Update animations
        for(size_t i = 0; i < 3; ++i) {
            ++alien_animation[i].time;
            if (alien_animation[i].time >= alien_animation[i].num_frames * alien_animation[i].frame_duration) {
                alien_animation[i].time = 0;
            }
        }
        ++alien_bullet_animation.time;
        if (alien_bullet_animation.time >= alien_bullet_animation.num_frames * alien_bullet_animation.frame_duration) {
            alien_bullet_animation.time = 0;
        }

        ++alien_update_timer;

        // Simulate player
        player_move_dir = 2 * move_dir;

        if (player_move_dir != 0) {
            if (game.player.x + player_sprite.width + player_move_dir >= game.width) {
                game.player.x = game.width - player_sprite.width;
            }
            else if ((int)game.player.x + player_move_dir <= 0) {
                game.player.x = 0;
            } else game.player.x += player_move_dir;
        }

        if (aliens_killed < game.num_aliens) {
            size_t ai = 0;
            while(game.aliens[ai].type == ALIEN_DEAD) ++ai;
            const Sprite& sprite = alien_sprites[2 * (game.aliens[ai].type - 1)];
            size_t pos = game.aliens[ai].x - (alien_death_sprite.width - sprite.width)/2;
            if(pos > alien_swarm_position) alien_swarm_position = pos;

            ai = game.num_aliens - 1;
            while(game.aliens[ai].type == ALIEN_DEAD) --ai;
            pos = game.width - game.aliens[ai].x - 13 + pos;
            if(pos > alien_swarm_max_position) alien_swarm_max_position = pos;
        } else {
            alien_update_frequency = 120;
            alien_swarm_position = 24;

            aliens_killed = 0;
            alien_update_timer = 0;

            alien_move_dir = 4;

            for (size_t xi = 0; xi < 11; ++xi) {
                for (size_t yi = 0; yi < 5; ++yi) {
                    size_t ai = xi * 5 + yi;

                    death_counters[ai] = 10;

                    Alien& alien = game.aliens[ai];
                    alien.type = (5 - yi) / 2 + 1;

                    const Sprite& sprite = alien_sprites[2 * (alien.type - 1)];

                    alien.x = 16 * xi + alien_swarm_position + (alien_death_sprite.width - sprite.width)/2;
                    alien.y = 17 * yi + 128;
                }
            }
        }

        // Process events
        if (fire_pressed && game.num_bullets < GAME_MAX_BULLETS) {
            game.bullets[game.num_bullets].x = game.player.x + player_sprite.width / 2;
            game.bullets[game.num_bullets].y = game.player.y + player_sprite.height;
            game.bullets[game.num_bullets].dir = 2;
            ++game.num_bullets;
        }
        fire_pressed = false;

        glfwPollEvents();
    }

    // clean exit
    glfwDestroyWindow(window);
    glfwTerminate();

    glDeleteVertexArrays(1, &fullscreen_triangle_vao);

    for (size_t i = 0; i < 6; ++i) {
        delete[] alien_sprites[i].data;
    }

    delete[] text_spritesheet.data;
    delete[] alien_death_sprite.data;
    delete[] player_bullet_sprite.data;
    delete[] alien_bullet_sprite[0].data;
    delete[] alien_bullet_sprite[1].data;
    delete[] alien_bullet_animation.frames;

    for (size_t i = 0; i < 3; ++i) {
        delete[] alien_animation[i].frames;
    }
    delete[] buffer.data;
    delete[] game.aliens;
    delete[] death_counters;

    return 0;
}