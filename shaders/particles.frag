#version 450 core

in vec4 v_Color;
in vec2 v_TexCoord;
in flat float v_TexIndex;

uniform sampler2D u_Textures[32];


layout (location = 0) out vec4 o_Color;

void main(){
	o_Color = texture2D( u_Textures[int(v_TexIndex)], v_TexCoord) * v_Color;
}
