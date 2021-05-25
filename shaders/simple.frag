#version 330 core

in vec3 interp_normal;
in vec2 interp_uv;

uniform vec4 color;
uniform bool use_tex;
uniform sampler2D texture_diffuse0;

out vec4 frag_color;

void main()
{
    if (use_tex)
    {
        frag_color = texture(texture_diffuse0, interp_uv);
    }
    else
    {
        frag_color = color;
    }
}
