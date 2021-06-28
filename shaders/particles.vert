#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in vec2 a_TexCoord1;
layout (location = 3) in vec2 a_TexCoord2;
layout (location = 4) in float a_HowMuchOfTexCoord1;
layout (location = 5) in float a_TexIndex;

uniform mat4 u_ViewProj;

out vec4 v_Color;
out vec2 v_TexCoord1;
out vec2 v_TexCoord2;
out float v_HowMuchOfTexCoord1;
out flat float v_TexIndex;

void main()
{
	gl_Position = u_ViewProj * vec4(a_Position, 1.0);

	v_Color = a_Color;
	v_TexCoord1 = a_TexCoord1;
	v_TexCoord2 = a_TexCoord2;
	v_HowMuchOfTexCoord1 = a_HowMuchOfTexCoord1;
	v_TexIndex = a_TexIndex;
}
