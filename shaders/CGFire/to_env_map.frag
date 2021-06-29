#version 450 core

in vec3 frag_pos;
in vec2 frag_uv;

uniform bool use_diffuse_tex;
uniform sampler2D diffuse_tex;
uniform vec4 diffuse_color;

out vec4 frag_color;

void main()
{
    if (use_diffuse_tex)
    {
        frag_color = vec4(texture(diffuse_tex, frag_uv).rgb, 1.0);
    }
    else
    {
        frag_color = diffuse_color;
    }
}
