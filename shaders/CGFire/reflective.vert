#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

out vec3 frag_pos;
out vec3 frag_normal;

uniform mat4 model_mat;
uniform mat3 normal_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;

void main()
{
    frag_normal = normal_mat * normal;
    frag_pos = vec3(model_mat * vec4(pos, 1.0));
    gl_Position = proj_mat * view_mat * vec4(frag_pos, 1.0);
}
