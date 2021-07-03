#version 450 core

layout (location = 0) in vec3 pos;

uniform float pixel_size;

out vec2 gauss_coords[5];

void main()
{
    gl_Position = vec4(pos, 1.0);
    vec2 center_coord = pos.xy * 0.5 + 0.5;
    for (int i = -2; i <= 2; i++)
        gauss_coords[i + 2] = center_coord + vec2(pixel_size * i, 0.0);
}