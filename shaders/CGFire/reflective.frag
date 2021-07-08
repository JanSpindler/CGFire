#version 450 core

in vec3 frag_pos;
in vec3 frag_normal;
in vec2 frag_uv;

uniform bool use_diffuse_tex;
uniform sampler2D diffuse_tex;
uniform vec4 diffuse_color;

uniform vec3 cam_pos;
uniform samplerCube skybox_tex;
uniform float reflectiveness;

out vec4 frag_color;

void main()
{
    vec3 I = normalize(frag_pos - cam_pos);
    vec3 R = normalize(reflect(I, normalize(frag_normal)));
    vec3 reflective_color = texture(skybox_tex, R).rgb;

    vec3 own_color = vec3(1.0);
    if (use_diffuse_tex)
    {
        own_color = texture(diffuse_tex, frag_uv).rgb;
    }
    else
    {
        own_color = diffuse_color.rgb;
    }

    vec3 final_color = reflectiveness * reflective_color + (1.0 - reflectiveness) * own_color;
    frag_color = vec4(final_color, 1.0);
}
