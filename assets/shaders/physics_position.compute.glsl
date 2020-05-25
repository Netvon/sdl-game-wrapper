#version 430

layout (std430, binding=1) buffer shader_data
{
  mat4 model[];
};

layout (std430, binding=2) buffer physics_data
{
  vec4 physics[];
};

layout( local_size_x = 1, local_size_y = 1, local_size_z = 1 ) in;
void main() {
	model[gl_GlobalInvocationID.x][3][0] += physics[gl_GlobalInvocationID.x].x;
	model[gl_GlobalInvocationID.x][3][1] += physics[gl_GlobalInvocationID.x].y;
}