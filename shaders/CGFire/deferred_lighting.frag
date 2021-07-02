#version 450 core

in vec2 frag_uv;

uniform sampler2D pos_tex;
uniform sampler2D normal_tex;
uniform sampler2D diffuse_tex;
uniform sampler2D specular_tex;

uniform vec3 cam_pos;

uniform vec3 dir_light_dir;
uniform vec3 dir_light_color;
uniform sampler2D dir_shadow_tex;
uniform mat4 dir_light_mat;

#define POINT_LIGHT_MAX 24
uniform int point_light_count;
uniform vec3 point_light_pos[POINT_LIGHT_MAX];
uniform vec3 point_light_color[POINT_LIGHT_MAX];
uniform float point_light_strength[POINT_LIGHT_MAX];
uniform samplerCube point_light_shadow_tex0;

uniform sampler2D ssao;

out vec4 out_color;

vec3 get_dir_light_color(vec3 pos, vec3 normal, vec3 diffuse_color, vec3 specular_color, float shininess, float ambientocclusion)
{
    vec3 light_dir = normalize(dir_light_dir);

    //Ambient
    vec3 ambient = vec3(diffuse_color*0.15*ambientocclusion);

    // Diffuse
    float diff = max(dot(normal, -light_dir), 0.0);
    vec3 diffuse_result = dir_light_color * (diff * diffuse_color);

    // Specular
    vec3 view_dir = normalize(cam_pos - pos);
    vec3 reflect_dir = reflect(light_dir, normal);
    float spec = 1.0;
    if (shininess > 0.0)
        spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
    vec3 specular_result = dir_light_color * spec * specular_color;

    // Shadow
    float shadow;

    vec4 dir_light_pos = dir_light_mat * vec4(pos, 1.0);

    vec3 proj_pos = dir_light_pos.xyz / dir_light_pos.w;
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
    return (ambient+shadow* (diffuse_result + specular_result));
}

vec3 get_point_light_color(vec3 pos, vec3 normal, vec3 diffuse_color, vec3 specular_color, float shininess, int index, float ambientocclusion)
{
    vec3 light_pos = point_light_pos[index];
    vec3 light_color = point_light_color[index];
    float light_strength = point_light_strength[index];

    vec3 light_dir = normalize(light_pos - pos);

    //Ambient
    vec3 ambient = vec3(diffuse_color*0.15*ambientocclusion);

    // Diffuse
    float diff = max(dot(normal, light_dir), 0.0);
    vec3 diffuse_result = light_color * (diff * diffuse_color);

    // Specular
    vec3 view_dir = normalize(cam_pos - pos);
    float distance = length(light_pos - pos);
    if (distance == 0.0)
        distance = 0.01;
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = 1.0;
    if (shininess > 0.0)
        spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
    vec3 specular_result = light_color * spec * specular_color;

    // Shadow
    vec3 frag_light_pos = pos - light_pos;
    float closest_depth = 1024.0;
    switch (index)
    {
        case 0:
            closest_depth = texture(point_light_shadow_tex0, frag_light_pos).r;
            break;
    }

    closest_depth *= 1024.0;
    float current_depth = length(frag_light_pos);
    float shadow = current_depth - 0.001 > closest_depth ? 0.0 : 1.0;

    float real_strength = light_strength / (distance * distance);
    return (ambient+shadow* (real_strength * (diffuse_result + specular_result)));
}

void main()
{
    // Retreive information from GBuffer
    vec3 frag_pos = texture(pos_tex, frag_uv).rgb;
    vec3 frag_normal = texture(normal_tex, frag_uv).rgb;
    vec3 diffuse_color = texture(diffuse_tex, frag_uv).rgb;
    vec4 specular_tex_val = texture(specular_tex, frag_uv);
    vec3 specular_color = specular_tex_val.rgb;
    float shininess = specular_tex_val.a;
    float ambientocclusion = texture(ssao, frag_uv).r;

    // Lighting
    vec3 result = get_dir_light_color(frag_pos, frag_normal, diffuse_color, specular_color, shininess, ambientocclusion);
    for (int i = 0; i < point_light_count; i++)
        result += get_point_light_color(frag_pos, frag_normal, diffuse_color, specular_color, shininess, i, ambientocclusion);

    out_color = vec4(result, 1.0);
}
