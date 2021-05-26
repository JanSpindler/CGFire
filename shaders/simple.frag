#version 330 core

// Input
in vec3 interp_model_pos;
in vec3 interp_normal;
in vec2 interp_uv;

// Directional Light
uniform vec3 dir_light_dir;
uniform vec4 dir_light_color;

// Point Lights
// TODO: implement

// Material
uniform float shininess;
uniform vec4 diffuse_color;
uniform vec4 specular_color;
uniform bool use_tex;
uniform sampler2D tex;

// Constants
const float pi = 3.14159265359;

// Output
out vec4 out_color;

void main()
{
    vec4 tex_color = vec4(1.0, 1.0, 1.0, 1.0);
    if (use_tex)
        tex_color = texture(tex, interp_uv);

    out_color = tex_color * diffuse_color;
}
