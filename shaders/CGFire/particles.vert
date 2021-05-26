#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;

uniform vec4 u_Color;
uniform mat4 u_ViewProj;
uniform mat4 u_Transform;

out vec4 color;
out vec2 texCoord;

void main()
{
	gl_Position = u_ViewProj * u_Transform * vec4(aPosition, 1.0);


	color = u_Color;
	texCoord = aTexCoord;
}
