#version 430
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main() {
	gl_Position = vec4(aPos.xy, 1.0, 1.0);
	TexCoord = aTexCoord;
}