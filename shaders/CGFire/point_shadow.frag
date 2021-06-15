#version 330 core

in vec4 frag_pos;

uniform vec3 light_pos;

void main()
{
    float light_distance = length(frag_pos.xyz - light_pos);
    light_distance = light_distance / 1024.0;
    gl_FragDepth = light_distance;
}
