#version 330 core

// Input
in vec3 interp_model_pos;
in vec3 interp_model_normal;
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
    vec4 diffuse = vec4(diffuse_color, 1.0);
    if (use_tex)
        diffuse = texture(tex, interp_uv);
    vec3 norm = normalize(interp_model_normal);
    float diff = max(dot(norm, dir_light_dir), 0.0);
    diffuse = vec4(dir_light_color, 1.0) * (diff * diffuse);

    // Specular


    // Result
    out_color = diffuse;
}
