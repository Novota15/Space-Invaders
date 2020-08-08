// validators header
#ifndef VALIDATORS_H
#define VALIDATORS_H

#include "main.h"

void validate_shader(GLuint shader, const char *file);

bool validate_program(GLuint program);

#endif