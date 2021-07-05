#version 450 core

in vec2 frag_uv;

uniform sampler2D og_tex;
uniform float strength;

out vec4 frag_color;

//https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
float rand(vec2 seed)
{
    return fract(sin(dot(seed, vec2(12.9898, 78.233))) * 43758.5453);
}

float grain_func(vec2 uv, vec4 src_color)
{
    float g0 = rand(uv);
    float g1 = rand(src_color.xy);
    float g2 = rand(src_color.zw);
    float grain = rand(vec2(g0 * g1, g1 * g2));

    return grain * strength;
}

void main()
{
    frag_color = texture(og_tex, frag_uv);
    vec4 grain = vec4(vec3(grain_func(frag_uv, frag_color)), 0.0);
    frag_color += grain;
}
