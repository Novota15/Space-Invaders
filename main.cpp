#include "main.h"

// global vars
bool game_running = false;
int move_dir = 0;
bool fire_pressed = 0;

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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    switch (key) {
    case GLFW_KEY_ESCAPE:
        if (action == GLFW_PRESS) game_running = false;
        break;
    case GLFW_KEY_RIGHT:
        if (action == GLFW_PRESS) move_dir += 1;
        else if(action == GLFW_RELEASE) move_dir -= 1;
        break;
    case GLFW_KEY_LEFT:
        if (action == GLFW_PRESS) move_dir -= 1;
        else if (action == GLFW_RELEASE) move_dir += 1;
        break;
    case GLFW_KEY_SPACE:
        if (action == GLFW_RELEASE) fire_pressed = true;
        break;
    default:
        break;
    }
}

void validate_shader(GLuint shader, const char *file = 0) {
    static const unsigned int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    GLsizei length = 0;

    glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer);

    if(length > 0) {
        printf("Shader %d(%s) compile error: %s\n", shader, (file? file: ""), buffer);
    }
}

bool validate_program(GLuint program) {
    static const GLsizei BUFFER_SIZE = 512;
    GLchar buffer[BUFFER_SIZE];
    GLsizei length = 0;

    glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer);

    if(length > 0) {
        printf("Program %d link error: %s\n", program, buffer);
        return false;
    }

    return true;
}

uint32_t rgb_to_uint32(uint8_t r, uint8_t g, uint8_t b) {
    return (r << 24) | (g << 16) | (b << 8) | 255;
}

int main(int argc, char* argv[]) {
    // error reporting
    glfwSetErrorCallback(error_callback);

    // initialize the GLFW library
    if (!glfwInit()) return -1;

    // give GLFW the appropriate hints before creating window
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // create game window
    GLFWwindow *window = glfwCreateWindow(640, 480, "Space Invaders", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return -1;
    }

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
    printf("Using OpenGL: %d.%d\n", glVersion[0], glVersion[1]);
    printf("Renderer used: %s\n", glGetString(GL_RENDERER));
    printf("Shading Language: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    glClearColor(1.0, 0.0, 0.0, 1.0);

    // create graphics buffer
    const size_t buffer_width = 224;
    const size_t buffer_height = 256;

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

    if(!validate_program(shader_id)) {
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

    Sprite bullet_sprite;
    bullet_sprite.width = 1;
    bullet_sprite.height = 3;
    bullet_sprite.data = new uint8_t[3]
    {
        1, // @
        1, // @
        1  // @
    };

    Sprite_Animation alien_animation[3];

    for(size_t i = 0; i < 3; ++i) {
        alien_animation[i].loop = true;
        alien_animation[i].num_frames = 2;
        alien_animation[i].frame_duration = 10;
        alien_animation[i].time = 0;

        alien_animation[i].frames = new Sprite*[2];
        alien_animation[i].frames[0] = &alien_sprites[2 * i];
        alien_animation[i].frames[1] = &alien_sprites[2 * i + 1];
    }

    Game game;
    game.width = buffer_width;
    game.height = buffer_height;
    game.num_aliens = 55;
    game.aliens = new Alien[game.num_aliens];

    game.player.x = 112 - 5;
    game.player.y = 32;

    game.player.life = 3;

    for(size_t yi = 0; yi < 5; ++yi) {
        for(size_t xi = 0; xi < 11; ++xi) {
            Alien& alien = game.aliens[yi * 11 + xi];
            alien.type = (5 - yi) / 2 + 1;

            const Sprite& sprite = alien_sprites[2 * (alien.type - 1)];

            alien.x = 16 * xi + 20 + (alien_death_sprite.width - sprite.width)/2;
            alien.y = 17 * yi + 128;
        }
    }

    uint8_t* death_counters = new uint8_t[game.num_aliens];
    for (size_t i = 0; i < game.num_aliens; ++i) {
        death_counters[i] = 10;
    }

    uint32_t clear_color = rgb_to_uint32(0, 128, 0);

    game_running = true;

    int player_move_dir = 0;

    // the game loop
    while (!glfwWindowShouldClose(window) && game_running) {
        buffer_clear(&buffer, clear_color);

        // Draw
        for (size_t ai = 0; ai < game.num_aliens; ++ai) {
            if (!death_counters[ai]) continue;

            const Alien& alien = game.aliens[ai];
            if (alien.type == ALIEN_DEAD) {
                buffer_draw_sprite(&buffer, alien_death_sprite, alien.x, alien.y, rgb_to_uint32(128, 0, 0));
            }
            else {
                const SpriteAnimation& animation = alien_animation[alien.type - 1];
                size_t current_frame = animation.time / animation.frame_duration;
                const Sprite& sprite = *animation.frames[current_frame];
                buffer_draw_sprite(&buffer, sprite, alien.x, alien.y, rgb_to_uint32(128, 0, 0));
            }
        }

        for (size_t bi = 0; bi < game.num_bullets; ++bi) {
            const Bullet& bullet = game.bullets[bi];
            const Sprite& sprite = bullet_sprite;
            buffer_draw_sprite(&buffer, sprite, bullet.x, bullet.y, rgb_to_uint32(128, 0, 0));
        }

        buffer_draw_sprite(&buffer, player_sprite, game.player.x, game.player.y, rgb_to_uint32(128, 0, 0));

        // Update animations
        for (size_t i = 0; i < 3; ++i) {
            ++alien_animation[i].time;
            if (alien_animation[i].time == alien_animation[i].num_frames * alien_animation[i].frame_duration) {
                alien_animation[i].time = 0;
            }
        }

        glTexSubImage2D(
            GL_TEXTURE_2D, 0, 0, 0,
            buffer.width, buffer.height,
            GL_RGBA, GL_UNSIGNED_INT_8_8_8_8,
            buffer.data
        );
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);

        // Simulate aliens
        for (size_t ai = 0; ai < game.num_aliens; ++ai) {
            const Alien& alien = game.aliens[ai];
            if (alien.type == ALIEN_DEAD && death_counters[ai]) {
                --death_counters[ai];
            }
        }

        // Simulate bullets
        for (size_t bi = 0; bi < game.num_bullets;) {
            game.bullets[bi].y += game.bullets[bi].dir;
            if(game.bullets[bi].y >= game.height || game.bullets[bi].y < bullet_sprite.height) {
                game.bullets[bi] = game.bullets[game.num_bullets - 1];
                --game.num_bullets;
                continue;
            }

            // Check hit
            for (size_t ai = 0; ai < game.num_aliens; ++ai) {
                const Alien& alien = game.aliens[ai];
                if (alien.type == ALIEN_DEAD) continue;

                const SpriteAnimation& animation = alien_animation[alien.type - 1];
                size_t current_frame = animation.time / animation.frame_duration;
                const Sprite& alien_sprite = *animation.frames[current_frame];
                bool overlap = sprite_overlap_check(
                    bullet_sprite, game.bullets[bi].x, game.bullets[bi].y,
                    alien_sprite, alien.x, alien.y
                );
                if (overlap) {
                    game.aliens[ai].type = ALIEN_DEAD;
                    // NOTE: Hack to recenter death sprite
                    game.aliens[ai].x -= (alien_death_sprite.width - alien_sprite.width)/2;
                    game.bullets[bi] = game.bullets[game.num_bullets - 1];
                    --game.num_bullets;
                    continue;
                }
            }

            ++bi;
        }

        // Simulate player
        player_move_dir = 2 * move_dir;

        if(player_move_dir != 0) {
            if(game.player.x + player_sprite.width + player_move_dir >= game.width) {
                game.player.x = game.width - player_sprite.width;
            }
            else if((int)game.player.x + player_move_dir <= 0) {
                game.player.x = 0;
            }
            else game.player.x += player_move_dir;
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

    delete[] alien_death_sprite.data;

    for (size_t i = 0; i < 3; ++i) {
        delete[] alien_animation[i].frames;
    }
    delete[] buffer.data;
    delete[] game.aliens;
    delete[] death_counters;

    return 0;
}