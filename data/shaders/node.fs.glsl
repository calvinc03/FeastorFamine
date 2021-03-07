#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform int highlight;
uniform float time;
uniform int num_monster_on_grid;

// Output color
layout(location = 0) out  vec4 color;

float min_dist_to_edge(vec2 uv) {
	float dist_to_top = uv.y;
	float dist_to_bottom = 1.0 - uv.y;
	float dist_to_left = uv.x;
	float dist_to_right = 1.0 - uv.x;
	return min(dist_to_top, min(dist_to_bottom, min(dist_to_left, dist_to_right)));
}

vec2 distort(vec2 uv)
{
	vec2 coord = uv;
	coord += (num_monster_on_grid) * vec2(sin(0.05*time + uv.y), cos(1234. + 0.025*time + 2.0*uv.x));
	coord += (num_monster_on_grid) * vec2(sin(10.*(0.005*time + uv.y)), cos(10.*(1234. + 0.025*time + 2.0*uv.x)));
	vec2 from = uv;
	vec2 to = coord;
	float fraction = clamp(0.1 * min_dist_to_edge(texcoord), 0.0, 1.0);
	return mix(from, to, fraction);
}

void main()
{
	vec2 coord = distort(texcoord);
	if (highlight == 0)
	{
		//color = vec4(1.0, 0.0, 0.0, 1.0);
		color = vec4(fcolor,1) * texture(sampler0, coord);

	}
	else
	{
		//color = vec4(1.0, 1.0,  0.0, 1.0);
		color = vec4(fcolor, 0.7) * texture(sampler0, coord);
		color += vec4(0.4, 0.3, 0.0, 0.3);
	}
}
