// validators header
#ifndef VALIDATORS_H
#define VALIDATORS_H

#include "main.h"

void validate_shader(GLuint shader, const char *file = 0);

bool validate_program(GLuint program);

#endif