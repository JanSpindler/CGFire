#version 330 core

// Input
in vec3 interp_pos;
in vec3 interp_normal;
in vec2 interp_uv;

// Directional Light
uniform vec3 dir_light_dir;
uniform vec3 dir_light_color;

// Point Lights
// TODO: implement

// Material
uniform float shininess;
uniform vec3 diffuse_color;
uniform vec3 specular_color;
uniform bool use_tex;
uniform sampler2D tex;

// Constants
const float pi = 3.14159265359;

// Output
out vec4 out_color;

void main()
{
    // Diffuse
    vec3 diffuse = diffuse_color;
    if (use_tex)
        diffuse = vec3(texture(tex, interp_uv));
    vec3 norm = normalize(interp_normal);
    float diff = max(dot(norm, -dir_light_dir), 0.0);
    diffuse = dir_light_color * (diff * diffuse);

    // Specular


    // Result
    out_color = vec4(diffuse, 1.0);
}
