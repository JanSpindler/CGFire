#version 330 core

in vec2 frag_uv;

uniform sampler2D pos_tex;
uniform sampler2D normal_tex;
uniform sampler2D color_tex;

out vec4 out_color;

void main()
{
    vec3 frag_pos = texture(pos_tex, frag_uv).rgb;
    vec3 frag_normal = texture(normal_tex, frag_uv).rgb;
    vec4 diff_spec_color = texture(color_tex, frag_uv);
    vec3 diffuse_color = diff_spec_color.rgb;
    float specular = diff_spec_color.a;

    out_color = vec4(diffuse_color, 1.0);
}
