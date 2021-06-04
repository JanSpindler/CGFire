#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;

uniform mat4 light_mat; // light_proj_mat * light_view_mat

out vec3 frag_pos;
out vec3 frag_normal;
out vec2 frag_uv;
out vec4 frag_light_pos;

void main()
{
    gl_Position = proj_mat * view_mat * model_mat * vec4(pos, 1.0);
    frag_pos = vec3(model_mat * vec4(pos, 1.0));
    frag_normal = mat3(transpose(inverse(model_mat))) * normal;
    frag_uv = uv;
    frag_light_pos = light_mat * vec4(frag_pos, 1.0);
}
