#version 330 

// Input attributes
//in vec3 in_position;
//in vec2 in_texcoord;
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_texcoord;
layout(location = 2) in vec2 center_point;

// Passed to fragment shader
out vec2 texcoord;

// Application data
mat3 transform = mat3(
  vec3(1000.0, 0.0, 0.0), // change
  vec3(0.0, 500.0, 0.0), // change
  vec3(center_point.x, center_point.y, 1.0)
);
uniform mat3 projection;

void main()
{
	texcoord = in_texcoord;
	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}
