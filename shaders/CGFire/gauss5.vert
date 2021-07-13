#version 450 core

layout (location = 0) in vec3 pos;

uniform float pixel_width;
uniform float pixel_height;

out vec2 gauss_coords[25];

void main()
{
    gl_Position = vec4(pos, 1.0);
    vec2 center_coord = pos.xy * 0.5 + 0.5;
    for (int y = 0; y < 5; y++)
    {
        for (int x = 0; x < 5; x++)
        {
            gauss_coords[y * 5 + x] = center_coord + vec2(pixel_width * (x - 2), pixel_height * (y - 2));
        }
    }
}