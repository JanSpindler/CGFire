#version 330 core

// Input
in vec3 interp_pos;
in vec3 interp_normal;
in vec2 interp_uv;

// Directional Light
uniform vec3 dir_light_dir;
uniform vec3 dir_light_color;

// Point Lights
#define POINT_LIGHT_COUNT 32
uniform vec3 point_light_pos[POINT_LIGHT_COUNT];
uniform vec3 point_light_color[POINT_LIGHT_COUNT];

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

vec4 get_dir_light_color()
{
    // Diffuse


    // Specular


    return vec4(0.0);
}

vec4 get_point_light_color()
{
    return vec4(0.0f);
}

void main()
{
    vec3 normal = normalize(interp_normal);

    // Diffuse
    vec4 diffuse = diffuse_color;
    if (use_tex)
        diffuse = texture(tex, interp_uv);
    float diff = max(dot(normal, normalize(-dir_light_dir)), 0.0);
    diffuse = vec4(dir_light_color, 1.0) * (diff * diffuse);

    // Specular


    // Result
    out_color = diffuse;
}
