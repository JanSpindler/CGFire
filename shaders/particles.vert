#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in float a_TexIndex;

uniform mat4 u_ViewProj;

out vec4 v_Color;
out vec2 v_TexCoord;
out flat float v_TexIndex;

void main()
{
	//We dont want any rotation of the camera to influence the particle appearance
	mat4 ViewProjMatNoRotation = mat4(
			vec4(1, 0, 0, u_ViewProj[0][3]),
			vec4(0, 1, 0, u_ViewProj[1][3]),
			vec4(0, 0, 1, u_ViewProj[2][3]), u_ViewProj[3]);

	gl_Position = u_ViewProj * vec4(a_Position, 1.0);

	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	v_TexIndex = a_TexIndex;
}
