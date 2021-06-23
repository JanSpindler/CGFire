#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

out vec3 frag_pos;
out vec3 frag_normal;
out vec2 frag_uv;

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;

void main()
{
    frag_pos = (model_mat * vec4(pos, 1.0)).xyz;
    frag_normal = transpose(inverse(model_mat)) * normal;
    frag_uv = uv;
    gl_Position = proj_mat * view_mat * model_mat * vec4(pos, 1.0);
}
