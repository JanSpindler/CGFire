#version 450 core

layout (location = 0) out float csm_sin;

const float pi = 3.14159265359;
const float c = 80.0;

void main()
{
    float z = gl_FragCoord.z;
    csm_sin = exp(c * z);
}
