#version 450 core

in vec2 frag_uv;

uniform sampler2D og_tex;
uniform float min_brightness;

out vec4 frag_color;

void main()
{
    vec3 og_color = texture(og_tex, frag_uv).rgb;
    float brightness = dot(og_color, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > min_brightness)
        frag_color = vec4(og_color, 1.0);
    else
        frag_color = vec4(0.0, 0.0, 0.0, 1.0);
}
