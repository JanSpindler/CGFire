#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

uniform mat4 shadow_view_mat;
uniform mat4 shadow_proj_mat;

void main()
{
    gl_Position = shadow_view_mat * shadow_proj_mat * vec4(pos, 1.0);
}
