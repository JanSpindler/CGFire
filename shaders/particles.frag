#version 450 core

in vec4 v_Color;
in vec2 v_TexCoord1; //Two Tex Coords, for blending between the two frames
in vec2 v_TexCoord2;
in float v_HowMuchOfTexCoord1;

in flat float v_TexIndex;

uniform sampler2D u_Textures[32];


layout (location = 0) out vec4 o_Color;

void main(){
	o_Color = mix(	texture( u_Textures[int(v_TexIndex)], v_TexCoord1),
					texture( u_Textures[int(v_TexIndex)], v_TexCoord2),
					v_HowMuchOfTexCoord1)
				* v_Color;
}
