#version 450 core

in vec3 frag_pos;
in vec3 frag_normal;
in vec2 frag_uv;

uniform bool use_diffuse_tex;
uniform sampler2D diffuse_tex;
uniform vec3 diffuse_color;

uniform bool use_specular_tex;
uniform sampler2D specular_tex;
uniform vec3 specular_color;

uniform float shininess;

layout (location = 0) out vec3 out_pos;
layout (location = 1) out vec3 out_normal;
layout (location = 2) out vec3 out_diffuse;
layout (location = 3) out vec4 out_specular;

void main()
{
    out_pos = frag_pos;

    out_normal = normalize(frag_normal);

    if (use_diffuse_tex)
        out_diffuse = texture(diffuse_tex, frag_uv).rgb;
    else
        out_diffuse = diffuse_color;

    if (use_specular_tex)
        out_specular.rgb = texture(specular_tex, frag_uv).rgb;
    else
        out_specular.rgb = specular_color;

    out_specular.a = shininess;
}
