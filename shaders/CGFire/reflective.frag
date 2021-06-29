#version 450 core

in vec3 frag_pos;
in vec3 frag_normal;

uniform vec3 cam_pos;
uniform samplerCube skybox_tex;

out vec4 frag_color;

void main()
{
    vec3 I = normalize(frag_pos - cam_pos);
    vec3 R = reflect(I, normalize(frag_normal));
    frag_color = vec4(texture(skybox_tex, R).rgb, 1.0);
}
