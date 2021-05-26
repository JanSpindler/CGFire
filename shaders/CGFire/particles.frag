#version 330 core

in vec4 color;
in vec2 texCoord;

uniform sampler2D u_Texture;

layout (location = 0) out vec4 o_Color;

void main(){

	// Output color = color of the texture at the specified UV
	o_Color = texture( u_Texture, texCoord) * color;
}
