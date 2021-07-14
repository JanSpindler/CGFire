#version 450 core

in vec4 frag_pos;

uniform vec3 light_pos;

//const float c = 80.0;

//layout (location = 0) out float esm;

void main()
{
    float light_distance = length(frag_pos.xyz - light_pos);
    light_distance = light_distance / 1024.0;
    gl_FragDepth = light_distance;

    //float z = light_distance;
    //esm = exp(c * z);
}
