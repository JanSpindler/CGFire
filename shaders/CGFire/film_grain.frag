#version 450 core

in vec2 frag_uv;

uniform sampler2D og_tex;
uniform float strength;
uniform vec2 rand_in;
uniform vec2 scree_size;

out vec4 frag_color;

//https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
float rand(vec2 seed)
{
    return fract(sin(dot(seed, vec2(12.9898, 78.233))) * 43758.5453);
}

float grain_func(vec2 uv)
{
    vec2 totalUv = vec2(uvec2(uv * scree_size / 2.0));
    float g0 = rand(totalUv / scree_size);
    float g1 = rand(rand_in);
    float grain = rand(vec2(g0, g1));

    return grain * strength;
}

void main()
{
    frag_color = texture(og_tex, frag_uv);
    vec4 grain = vec4(vec3(grain_func(frag_uv)), 0.0);
    frag_color += grain;
}
