// Source Tutorial: https://learnopengl.com/In-Practice/Debugging
// This will give us OpenGL Error Messages

#pragma once

#include <glad/glad.h>
#include <iostream>

#define ASSERT(x) if(!(x)) __debugbreak();

namespace util {

	void EnableGLDebugging();

}