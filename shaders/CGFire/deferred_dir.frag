#version 450 core

in vec2 frag_uv;

uniform sampler2D pos_tex;
uniform sampler2D normal_tex;
uniform sampler2D diffuse_tex;
uniform sampler2D specular_tex;

uniform vec3 cam_pos;

uniform bool use_esm;

uniform vec3 light_dir;
uniform vec3 light_color;
uniform sampler2D shadow_tex;
uniform sampler2D shadow_esm_tex;
uniform mat4 light_mat;

const float pi = 3.14159265359;
const float c = 80.0;

out vec4 frag_color;

float get_dir_shadow(vec3 pos)
{
    vec4 dir_light_pos = light_mat * vec4(pos, 1.0);
    vec3 proj_pos = dir_light_pos.xyz / dir_light_pos.w;
    proj_pos = proj_pos * 0.5 + 0.5;

    if (use_esm)
    {
        float d = proj_pos.z;
        float exp_z = texture(shadow_esm_tex, proj_pos.xy).r;
        float exp_d = exp(-c * d);
        float result = exp_d * exp_z;
        return clamp(result, 0.0, 1.0);
    }
    else
    {
        if (proj_pos.z > 1.0)
        {
            return 1.0;
        }
        else
        {
            float closest_depth = texture(shadow_tex, proj_pos.xy).r;
            float current_depth = proj_pos.z;
            return current_depth - 0.001 > closest_depth ? 0.0 : 1.0;
        }
    }
}

vec3 get_dir_light_color(vec3 pos, vec3 normal, vec3 diffuse_color, vec3 specular_color, float shininess)
{
    vec3 light_dir_n = normalize(light_dir);

    // Diffuse
    float diff = max(dot(normal, -light_dir_n), 0.0);
    vec3 diffuse_result = light_color * (diff * diffuse_color);

    // Specular
    vec3 view_dir = normalize(cam_pos - pos);
    vec3 reflect_dir = reflect(light_dir_n, normal);
    float spec = 1.0;
    if (shininess > 0.0)
        spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
    vec3 specular_result = light_color * spec * specular_color;

    // Shadow
    float shadow = get_dir_shadow(pos);
    return shadow * (diffuse_result + specular_result);
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

    // Calculate light shadow
    vec3 light_result = get_dir_light_color(frag_pos, frag_normal, diffuse_color, specular_color, shininess);

    // Final result
    frag_color = vec4(light_result, 1.0);
}