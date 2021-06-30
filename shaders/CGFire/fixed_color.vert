#version 450 core

layout (location = 0) in vec3 pos;

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;

void main()
{
    gl_Position = proj_mat * view_mat * model_mat * vec4(pos, 1.0);
}
