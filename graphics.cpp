// game graphics functions
#include "graphics.h"

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
    // tell GFLW to make subsequent OpenGL calls apply to current 
    glfwMakeContextCurrent(window);
    return window;
}

void query_and_report_opengl_versions() {
    int glVersion[2] = {-1,1};
    glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]);
    glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]);

    gl_debug(__FILE__, __LINE__);

    // report versions used
    printf("Playing Space Invaders, created by Grant Novota\n");
    printf("Using OpenGL: %d.%d\n", glVersion[0], glVersion[1]);
    printf("Renderer used: %s\n", glGetString(GL_RENDERER));
    printf("Shading Language: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    return;
}

uint32_t rgb_to_uint32(uint8_t r, uint8_t g, uint8_t b) {
    return (r << 24) | (g << 16) | (b << 8) | 255;
}

void buffer_draw_number(Buffer* buffer, const Sprite& number_spritesheet,
    size_t number, size_t x, size_t y, uint32_t color) {
    uint8_t digits[64];
    size_t num_digits = 0;

    size_t current_number = number;
    do {
        digits[num_digits++] = current_number % 10;
        current_number = current_number / 10;
    }
    while(current_number > 0);

    size_t xp = x;
    size_t stride = number_spritesheet.width * number_spritesheet.height;
    Sprite sprite = number_spritesheet;
    for(size_t i = 0; i < num_digits; ++i) {
        uint8_t digit = digits[num_digits - i - 1];
        sprite.data = number_spritesheet.data + digit * stride;
        buffer_draw_sprite(buffer, sprite, xp, y, color);
        xp += sprite.width + 1;
    }
}

void buffer_draw_text(Buffer* buffer, const Sprite& text_spritesheet,
    const char* text, size_t x, size_t y, uint32_t color) {
    size_t xp = x;
    size_t stride = text_spritesheet.width * text_spritesheet.height;
    Sprite sprite = text_spritesheet;
    for (const char* charp = text; *charp != '\0'; ++charp) {
        char character = *charp - 32;
        if(character < 0 || character >= 65) continue;

        sprite.data = text_spritesheet.data + character * stride;
        buffer_draw_sprite(buffer, sprite, xp, y, color);
        xp += sprite.width + 1;
    }
}