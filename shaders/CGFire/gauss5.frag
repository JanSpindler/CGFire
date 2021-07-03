#version 450 core

in vec2 gauss_coords[5];

uniform sampler2D tex;

const float weights[] = { 0.06136, 0.24477, 0.38774, 0.24477, 0.06136 };

out vec4 frag_color;

void main()
{
    vec3 result = vec3(0.0);

    for (int i = 0; i < 5; i++)
        result += texture(tex, gauss_coords[i]).rgb * weights[i];

    frag_color = vec4(result, 1.0);
}