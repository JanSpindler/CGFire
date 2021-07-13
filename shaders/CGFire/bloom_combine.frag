#version 450 core

in vec2 frag_uv;

uniform sampler2D og_tex;
uniform sampler2D bloom_tex;

out vec4 frag_color;

void main()
{
    vec3 og_color = texture(og_tex, frag_uv).rgb;
    vec3 bloom_color = texture(bloom_tex, frag_uv).rgb;
    frag_color = vec4(og_color + bloom_color, 1.0);
}
