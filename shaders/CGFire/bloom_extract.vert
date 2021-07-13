#version 450 core

layout (location = 0) in vec3 pos;

out vec2 frag_uv;

void main()
{
    gl_Position = vec4(pos, 1.0);
    frag_uv = pos.xy * 0.5 + 0.5;
}
