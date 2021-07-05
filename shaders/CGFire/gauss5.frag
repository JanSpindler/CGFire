#version 450 core

in vec2 gauss_coords[25];

uniform sampler2D og_tex;

const float weights[] = {
    0.003765, 0.015019, 0.023792, 0.015019, 0.003765,
    0.015019, 0.059912,	0.094907, 0.059912, 0.015019,
    0.023792, 0.094907,	0.150342, 0.094907, 0.023792,
    0.015019, 0.059912,	0.094907, 0.059912, 0.015019,
    0.003765, 0.015019,	0.023792, 0.015019, 0.003765
};

out vec4 frag_color;

void main()
{
    vec3 result = vec3(0.0);

    for (int y = 0; y < 5; y++)
    {
        for (int x = 0; x < 5; x++)
        {
            int index = y * 5 + x;
            result += texture(og_tex, gauss_coords[index]).rgb * weights[index];
        }
    }

    frag_color = vec4(result, 1.0);
}