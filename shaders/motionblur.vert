#version 330 core

layout (location = 0) in vec3 Position;

out vec2 TexCoord0;

void main(){
    gl_Position = vec4(Position, 1.0f);
    TexCoord0 = clamp(0.5 * (Position.xy + 1.0), 0.0, 1.0);
}