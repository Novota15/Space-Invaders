#include <cstudio>
#include <GLFW/glfw3.h>

// error events reported through callbacks
void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

int main(int argc, char* argv[]) {
    glfwSetErrorCallback(error_callback);
    return 0;
}