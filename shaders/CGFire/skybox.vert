#version 450 core

layout (location = 0) in vec3 pos;

uniform mat4 view_mat;
uniform mat4 proj_mat;

out vec3 dir;

void main()
{
    dir = pos;
    vec4 pre_pos = proj_mat * view_mat * vec4(pos, 1.0);
    gl_Position = pre_pos.xyww; // z will always be 1.0
}
