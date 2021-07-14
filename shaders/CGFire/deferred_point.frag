#version 450 core

in vec2 frag_uv;

uniform sampler2D pos_tex;
uniform sampler2D normal_tex;
uniform sampler2D diffuse_tex;
uniform sampler2D specular_tex;

uniform sampler2D old_tex;

uniform vec3 cam_pos;

uniform bool use_esm;

uniform vec3 light_pos;
uniform vec3 light_color;
uniform float light_strength;
uniform samplerCube shadow_tex;
//uniform samplerCube shadow_esm_tex;

const float pi = 3.14159265359;
const float c = 80.0;

out vec4 frag_color;

float get_point_shadow(vec3 frag_light_pos)
{
    // Shadow
    float closest_depth = texture(shadow_tex, frag_light_pos).r;
    float exp_z = 1.0;// = texture(shadow_esm_tex, frag_light_pos).r;

    float shadow;
    float current_depth = length(frag_light_pos);
    if (use_esm)
    {
        exp_z = exp(c * closest_depth); // not multiplied by 1024.0!!! // This is not a perfect solution but works
        float d = current_depth / 1024.0;
        float exp_d = exp(-c * d);
        float result = exp_d * exp_z;
        shadow = clamp(result, 0.0, 1.0);
    }
    else
    {
        closest_depth *= 1024.0;
        shadow = current_depth - 0.001 > closest_depth ? 0.0 : 1.0;
    }

    return shadow;
}

vec3 get_point_light_color(vec3 pos, vec3 normal, vec3 diffuse_color, vec3 specular_color, float shininess)
{
    vec3 light_dir = normalize(light_pos - pos);

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
    float shadow = get_point_shadow(frag_light_pos);

    // Result
    float real_strength = light_strength / (distance * distance);
    return shadow * real_strength * (diffuse_result + specular_result);
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

    // Calculate color
    vec3 old_result = vec3(get_point_shadow(frag_pos - light_pos));//texture(old_tex, frag_uv).rgb;
    vec3 light_result = get_point_light_color(frag_pos, frag_normal, diffuse_color, specular_color, shininess);
    frag_color = vec4(old_result + light_result, 1.0);
}
