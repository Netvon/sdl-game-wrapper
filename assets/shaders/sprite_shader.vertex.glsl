#version 430
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 in_color;
layout (location = 2) in vec2 aTexCoord;

out vec3 out_color;
out vec2 TexCoord;
//uniform mat4 transform;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

void main() {
	gl_Position = projection * view * model * vec4(aPos.xy, 0.0, 1.0);
	out_color = in_color;
	TexCoord = aTexCoord;
}