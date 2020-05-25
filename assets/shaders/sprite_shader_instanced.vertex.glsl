#version 430
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

struct sprite_data {
	vec4 translate;
	vec2 scale;
	float rotation;
	vec2 uvs[4];
	vec4 tint;
};

layout (std430, binding=1) buffer shader_data
{
  mat4 model[];
};

layout (std430, binding=2) buffer color_data
{
  vec4 colors[];
};

layout (std430, binding=3) buffer main_sprite_data
{
  sprite_data data[];
};

out vec3 out_color;
out vec2 TexCoord;
uniform mat4 projection;
uniform mat4 view;

void main() {
	gl_Position = projection * view * model[gl_InstanceID] * vec4(aPos.xy, 0.0, 1.0);
	out_color = colors[gl_InstanceID].xyz;
	TexCoord = aTexCoord;
}