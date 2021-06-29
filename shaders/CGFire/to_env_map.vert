#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 2) in vec2 uv;

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;

out vec3 frag_pos;
out vec2 frag_uv;

void main()
{
    frag_pos = vec3(model_mat * vec4(pos, 1.0));
    frag_uv = uv;
    gl_Position = proj_mat * view_mat * model_mat * vec4(pos, 1.0);
}
