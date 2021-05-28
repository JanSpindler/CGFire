#version 330 core

// Input
in vec3 interp_pos;
in vec3 interp_normal;
in vec2 interp_uv;

uniform vec3 cam_pos;

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
    vec3 view_dir = normalize(cam_pos - interp_pos);
    vec3 reflect_dir = reflect(normalize(dir_light_dir), normal);
    float spec = 1.0;
    if (shininess > 0.0)
        spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
    vec4 specular = vec4(dir_light_color, 1.0) * spec * specular_color;

    // Result
    out_color = diffuse + specular;
}
