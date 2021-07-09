#version 450 core

in vec3 frag_pos;
in vec3 frag_normal;
in vec2 frag_uv;

uniform bool use_diffuse_tex;
uniform sampler2D diffuse_tex;
uniform vec4 diffuse_color;

uniform bool use_specular_tex;
uniform sampler2D specular_tex;
uniform vec4 specular_color;

uniform bool use_normal_tex;
uniform sampler2D normal_tex;

uniform float shininess;

layout (location = 0) out vec3 out_pos;
layout (location = 1) out vec3 out_normal;
layout (location = 2) out vec3 out_diffuse;
layout (location = 3) out vec4 out_specular;

void main()
{
    // Pos
    out_pos = frag_pos;

    // Normal
    vec3 normal = normalize(frag_normal);
    if (use_normal_tex)
    {
        vec3 tex_normal = texture(normal_tex, frag_uv).rgb;
        tex_normal = normalize(tex_normal * 2.0 - 1.0);
    }
    out_normal = normal;

    // Diffuse color
    if (use_diffuse_tex)
        out_diffuse = texture(diffuse_tex, frag_uv).rgb;
    else
        out_diffuse = vec3(diffuse_color);

    // Specular color
    if (use_specular_tex)
        out_specular.rgb = texture(specular_tex, frag_uv).rgb;
    else
        out_specular.rgb = vec3(specular_color);

    // Shininess
    out_specular.a = shininess;
}
