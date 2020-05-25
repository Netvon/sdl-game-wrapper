#version 430

layout (std430, binding=1) buffer shader_data
{
  mat4 model[];
};

layout (std430, binding=2) buffer physics_data
{
  vec4 physics[];
};

uniform float time_step;

float distance2(vec2 pos_a, vec2 pos_b) {
  vec2 dist = pos_b - pos_a;
  return dist.x * dist.x + dist.y * dist.y;
}

bool intersect(vec2 pos_a, vec2 pos_b) {
  float r = 4096;

  return r < distance2(pos_a, pos_b);
}

layout( local_size_x = 1, local_size_y = 1, local_size_z = 1 ) in;
void main() {
    if(gl_GlobalInvocationID.x != gl_GlobalInvocationID.y) {
        const vec2 pos_a = model[gl_GlobalInvocationID.x][3].xy;
        const vec2 pos_b = model[gl_GlobalInvocationID.y][3].xy;

        const float radius_a = model[gl_GlobalInvocationID.x][0].x * 0.5;
        const float radius_b = model[gl_GlobalInvocationID.y][0].x * 0.5;

        const float mass_a = physics[gl_GlobalInvocationID.x].z;
        const float mass_b = physics[gl_GlobalInvocationID.y].z;

        const vec2 velo_a = physics[gl_GlobalInvocationID.x].xy;
        const vec2 velo_b = physics[gl_GlobalInvocationID.y].xy;

        const vec2 dist = pos_b - pos_a;
        const float dist_sqr = (dist.x * dist.x + dist.y * dist.y) - (radius_a * radius_a + radius_b * radius_b);

        if(dist_sqr > 0.1) {
          const vec2 force_direction = normalize(dist);
          const vec2 force = force_direction * 0.000000000066742 * mass_a * mass_b / dist_sqr;
          const vec2 acceleration = force / mass_a;

          physics[gl_GlobalInvocationID.x].xy += acceleration * time_step;
        } else {
          const vec2 normal = normalize(dist);
          const vec2 relative_v = velo_b - velo_a;
          const float velo_along_normal = dot(relative_v, normal);

          if(velo_along_normal <= 0) {
            float e = min(physics[gl_GlobalInvocationID.x].w, physics[gl_GlobalInvocationID.y].w);
            float j = -(1.0 + e) * velo_along_normal;
            j /= (1.0 / mass_a) + (1.0 / mass_b);

            vec2 impulse = j * normal;
            physics[gl_GlobalInvocationID.x].xy += 1.0 / mass_a * impulse * time_step;
            physics[gl_GlobalInvocationID.y].xy -= 1.0 / mass_b * impulse * time_step;

            // physics[gl_GlobalInvocationID.x].x = 0;
            // physics[gl_GlobalInvocationID.x].y = 0;
          }
        }
        // } else {
        //   vec2 normal = normalize(dist);
        //   vec2 relative_v = velo_b - velo_a;

        //   float velo_along_normal = dot(relative_v, normal);
          
        //   if(velo_along_normal < 0) {

        //     float e = min(physics[gl_GlobalInvocationID.x].w, physics[gl_GlobalInvocationID.y].w);
        //     float j = -(1.0 + e) * velo_along_normal;
        //     j /= 1.0 / mass_a + 1.0 / mass_b;

        //     vec2 impulse = j * normal;
        //     physics[gl_GlobalInvocationID.x].xy -= 1.0 / mass_a * impulse * time_step;
        //     // physics[gl_GlobalInvocationID.y].xy += 1.0 / mass_b * impulse * time_step;
        //   }
        // }
    }
}