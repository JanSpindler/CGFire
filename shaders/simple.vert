#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;

out vec3 interp_model_pos;
out vec3 interp_model_normal;
out vec2 interp_uv;

void main()
{
    gl_Position = proj_mat * view_mat * model_mat * vec4(pos, 1.0);
    interp_model_pos = (model_mat * vec4(pos, 1.0)).xyz;
    interp_model_normal = (model_mat * vec4(normal, 1.0)).xyz;;
    interp_uv = uv;
}
