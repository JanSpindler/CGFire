#version 330 core

layout (location = 0) in vec3 pos;

uniform mat4 model_mat;

void main()
{
    gl_Position = model_mat * vec4(pos, 1.0);
}
