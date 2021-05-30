#version 330 core

// input
in vec3 interp_pos;
in vec3 interp_normal;
in vec2 interp_uv;

uniform vec3 cam_pos;

// Directional Light
uniform vec3 dir_light_dir;
uniform vec3 dir_light_color;

// Point Lights
#define POINT_LIGHT_MAX 128
uniform uint point_light_count;
uniform vec3 point_light_pos[POINT_LIGHT_MAX];
uniform vec3 point_light_color[POINT_LIGHT_MAX];
uniform float point_light_strength[POINT_LIGHT_MAX];

// Material
uniform float mat_shininess;
uniform vec4 mat_diffuse_color;
uniform vec4 mat_specular_color;
uniform bool mat_use_tex;
uniform sampler2D mat_tex;

// Constants
const float pi = 3.14159265359;

// Output
out vec4 out_color;

vec4 get_dir_light_color(vec3 normal)
{
    // Diffuse
    vec4 diffuse = mat_diffuse_color;
    if (mat_use_tex)
    diffuse = texture(mat_tex, interp_uv);
    float diff = max(dot(normal, normalize(-dir_light_dir)), 0.0);
    diffuse = vec4(dir_light_color, 1.0) * (diff * diffuse);

    // Specular
    vec3 view_dir = normalize(cam_pos - interp_pos);
    vec3 reflect_dir = reflect(normalize(dir_light_dir), normal);
    float spec = 1.0;
    if (mat_shininess > 0.0)
    spec = pow(max(dot(view_dir, reflect_dir), 0.0), mat_shininess);
    vec4 specular = vec4(dir_light_color, 1.0) * spec * mat_specular_color;

    return diffuse + specular;
}

vec4 get_point_light_color(vec3 normal, uint index)
{
    vec3 light_pos = point_light_pos[index];
    vec3 light_color = point_light_color[index];
    float light_strength = point_light_strength[index];

    vec3 light_dir = normalize(light_pos - interp_pos);

    // Diffuse
    vec4 diffuse = mat_diffuse_color;
    if (mat_use_tex)
    diffuse = texture(mat_tex, interp_uv);
    float diff = max(dot(normal, light_dir), 0.0);
    diffuse = vec4(light_color, 1.0) * (diff * diffuse);

    // Specular
    vec3 view_dir = normalize(cam_pos - interp_pos);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = 1.0;
    if (mat_shininess > 0.0)
    spec = pow(max(dot(view_dir, reflect_dir), 0.0), mat_shininess);
    vec4 specular = vec4(light_color, 1.0) * spec * mat_specular_color;

    return light_strength * (diffuse + specular);
}

void main()
{
    vec3 normal = normalize(interp_normal);

    vec4 result = get_dir_light_color(normal);
    for (uint i = 0u; i < point_light_count; i++)
        result += get_point_light_color(normal, i);

    out_color = result;
}
