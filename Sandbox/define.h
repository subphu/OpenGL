//  Copyright © 2019 Subph. All rights reserved.
//

#pragma once

#define GLEW_STATIC

#include <GL/glew.h>
#include <iostream>

#define PI 3.14159265358979323846

#define LOG std::cout <<
#define ENDL << std::endl
#define SPACE << " " <<

#if __APPLE__
#define GLSL_VERSION "#version 150"
#else
#define GLSL_VERSION "#version 130";
#endif

template<typename T> struct Size {
    T width, height;
};
