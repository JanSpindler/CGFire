#version 450 core

in vec3 dir;

uniform samplerCube skybox_tex;

out vec4 frag_color;

void main()
{
    frag_color = texture(skybox_tex, dir);
}
