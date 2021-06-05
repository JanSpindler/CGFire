#version 330 core

// input
in vec3 frag_pos;
in vec3 frag_normal;
in vec2 frag_uv;
in vec4 frag_dir_light_pos;

uniform vec3 cam_pos;

// Directional Light
uniform vec3 dir_light_dir;
uniform vec3 dir_light_color;

// Point Lights
#define POINT_LIGHT_MAX 338u
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

// Shadow
uniform sampler2D shadow_tex;

// Constants
const float pi = 3.14159265359;

// Output
out vec4 out_color;

float is_in_shadow(vec4 light_pos)
{
    vec3 proj_pos = light_pos.xyz / light_pos.w;
    proj_pos = proj_pos * 0.5 + 0.5;

    if (proj_pos.z > 1.0)
        return 0.0;

    float closest_depth = texture(shadow_tex, proj_pos.xy).r;
    float current_depth = proj_pos.z;
    float bias = 0.001;
    float shadow = current_depth - bias > closest_depth ? 1.0 : 0.0;

    return shadow;
}

vec4 get_dir_light_color(vec3 normal)
{
    vec3 light_dir = normalize(dir_light_dir);

    // Diffuse
    vec4 diffuse = mat_diffuse_color;
    if (mat_use_tex)
        diffuse = texture(mat_tex, frag_uv);
    float diff = max(dot(normal, -light_dir), 0.0);
    diffuse = vec4(dir_light_color, 1.0) * (diff * diffuse);

    // Specular
    vec3 view_dir = normalize(cam_pos - frag_pos);
    vec3 reflect_dir = reflect(light_dir, normal);
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

    vec3 light_dir = normalize(light_pos - frag_pos);

    // Diffuse
    vec4 diffuse = mat_diffuse_color;
    if (mat_use_tex)
        diffuse = texture(mat_tex, frag_uv);
    float diff = max(dot(normal, light_dir), 0.0);
    diffuse = vec4(light_color, 1.0) * (diff * diffuse);

    // Specular
    vec3 view_dir = normalize(cam_pos - frag_pos);
    float distance = length(light_pos - frag_pos);
    if (distance == 0.0)
        distance = 0.01;
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = 1.0;
    if (mat_shininess > 0.0)
        spec = pow(max(dot(view_dir, reflect_dir), 0.0), mat_shininess);
    vec4 specular = vec4(light_color, 1.0) * spec * mat_specular_color;

    float real_strength = light_strength / (distance * distance);
    return real_strength * (diffuse + specular);
}

void main()
{
    vec3 normal = normalize(frag_normal);

    vec4 result = (1.0 - is_in_shadow(frag_dir_light_pos)) *  get_dir_light_color(normal);

    uint real_point_light_count;
    if (point_light_count > POINT_LIGHT_MAX)
        real_point_light_count = POINT_LIGHT_MAX;
    else
        real_point_light_count = point_light_count;
    for (uint i = 0u; i < real_point_light_count; i++)
        result += get_point_light_color(normal, i);

    out_color = result;
}
