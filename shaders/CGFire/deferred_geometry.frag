#version 330 core

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
layout (location = 2) out vec4 out_color;

void main()
{
    out_pos = frag_pos;
    out_normal = normalize(frag_normal);
    out_color.rgb = texture(diffuse_tex, frag_uv).rgb;
    out_color.a = texture(specular_tex, frag_uv).r;
}
