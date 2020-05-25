#version 430
#define M_PI 3.1415926535897932384626433832795
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

struct sprite_data {
	vec4 translate;
	vec4 scale_rotation;
	vec4 tint;
	vec2 uvs[4];
};

layout (std430, binding=0) buffer main_sprite_data
{
  sprite_data data[];
};

out vec3 out_color;
out vec2 TexCoord;
uniform mat4 projection;
uniform mat4 view;

mat4 translate(mat4 m, vec3 v) {
	mat4 Result = m;
	Result[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
	return Result;
}

mat4 scale(mat4 m, vec3 v) {
	mat4 Result;
	Result[0] = m[0] * v[0];
	Result[1] = m[1] * v[1];
	Result[2] = m[2] * v[2];
	Result[3] = m[3];
	return Result;
}

mat4 rotate(mat4 m, float angle, vec3 v) {
	float a = (M_PI / 180.0) * angle;
	float c = cos(a);
	float s = sin(a);

	vec3 axis = normalize(v);
	vec3 temp = (1.0 - c) * axis;

	mat4 Rotate;
	Rotate[0][0] = c + temp[0] * axis[0];
	Rotate[0][1] = temp[0] * axis[1] + s * axis[2];
	Rotate[0][2] = temp[0] * axis[2] - s * axis[1];

	Rotate[1][0] = temp[1] * axis[0] - s * axis[2];
	Rotate[1][1] = c + temp[1] * axis[1];
	Rotate[1][2] = temp[1] * axis[2] + s * axis[0];

	Rotate[2][0] = temp[2] * axis[0] + s * axis[1];
	Rotate[2][1] = temp[2] * axis[1] - s * axis[0];
	Rotate[2][2] = c + temp[2] * axis[2];

	mat4 Result;
	Result[0] = m[0] * Rotate[0][0] + m[1] * Rotate[0][1] + m[2] * Rotate[0][2];
	Result[1] = m[0] * Rotate[1][0] + m[1] * Rotate[1][1] + m[2] * Rotate[1][2];
	Result[2] = m[0] * Rotate[2][0] + m[1] * Rotate[2][1] + m[2] * Rotate[2][2];
	Result[3] = m[3];
	return Result;
}

void main() {
	sprite_data instance = data[gl_InstanceID];

	mat4 m = mat4(1.0);
	mat4 m1 = translate(m, instance.translate.xyz);
	mat4 m2 = rotate(m1, instance.scale_rotation.z, vec3(0.0, 0.0, 1.0));
	mat4 m3 = scale(m2, vec3(instance.scale_rotation.xy, 1.0));

	gl_Position = projection * view * m3 * vec4(aPos.xy, 0.0, 1.0);
	out_color = instance.tint.xyz;
	TexCoord = instance.uvs[gl_VertexID].xy;
}