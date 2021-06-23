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
uniform sampler2D dir_shadow_tex;

// Point Lights
#define POINT_LIGHT_MAX 24u
uniform uint point_light_count;
uniform vec3 point_light_pos[POINT_LIGHT_MAX];
uniform vec3 point_light_color[POINT_LIGHT_MAX];
uniform float point_light_strength[POINT_LIGHT_MAX];
uniform samplerCube point_light_shadow_cube0;
// TODO: more samplerCubes

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

float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
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

    // Shadow
    float shadow;

    vec3 proj_pos = frag_dir_light_pos.xyz / frag_dir_light_pos.w;
    proj_pos = proj_pos * 0.5 + 0.5;

    if (proj_pos.z > 1.0)
    {
        shadow = 1.0;
    }
    else
    {
        float closest_depth = texture(dir_shadow_tex, proj_pos.xy).r;
        float current_depth = proj_pos.z;
        shadow = current_depth - 0.001 > closest_depth ? 0.0 : 1.0;
    }
    return shadow * (diffuse + specular);
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

    // Shadow
    vec3 frag_light_pos = frag_pos - light_pos;

    float closest_depth = 1000.0;
    switch (index)
    {
        case 0u:
            closest_depth = texture(point_light_shadow_cube0, frag_light_pos).r;
            break;
        // TODO: implement more cubemaps
    }

    closest_depth *= 1024.0;
    float current_depth = length(frag_light_pos);
    float shadow = current_depth - 0.001 > closest_depth ? 0.0 : 1.0;

    float real_strength = light_strength / (distance * distance);
    return shadow * (real_strength * (diffuse + specular));
}

void main()
{
    vec3 normal = normalize(frag_normal);

    vec4 result = get_dir_light_color(normal);

    for (uint i = 0u; i < point_light_count; i++)
        result += get_point_light_color(normal, i);

    out_color = result;
}
