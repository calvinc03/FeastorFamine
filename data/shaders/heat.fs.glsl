#version 330

uniform sampler2D screen_texture;
uniform float time;
uniform float darken_screen_factor;

in vec2 texcoord;

layout(location = 0) out vec4 color;

float min_dist_to_edge(vec2 uv) {
	float dist_to_top = uv.y -.1;
	float dist_to_bottom = 1.0 - uv.y - .1;
	float dist_to_left = uv.x;
	float dist_to_right = 1.0 - uv.x;

	return min(dist_to_top, min(dist_to_bottom, min(dist_to_left, dist_to_right)));
}

vec2 distort(vec2 uv)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// HANDLE THE WATER WAVE DISTORTION HERE (you may want to try sin/cos)
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	vec2 coord = uv.xy;
	coord += 0.004*vec2(sin(0.05*time + 1.*uv.y), cos(1234. + 0.025*time + 2.*uv.x));
	coord += 0.002*vec2(sin(10.*(0.05*time + 1.*uv.y)), cos(10.*(1234. + 0.025*time + 2.*uv.x)));
	
	vec2 from = uv;
	vec2 to = coord;
	float fraction = clamp(10.0 * min_dist_to_edge(texcoord), 0.0, 1.0);

	return mix(from, to, fraction);

	// return coord;
}

vec4 color_shift(vec4 in_color)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// HANDLE THE COLOR SHIFTING HERE (you may want to make it blue-ish)
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	vec4 color = mix(in_color, vec4(0.9, 0.2, 0.2, 1.0), 0.2);
	
	vec4 from = in_color;
	vec4 to = color;
	float fraction = clamp(10.0 * min_dist_to_edge(texcoord), 0.0, 1.0);

	return mix(from, to, fraction);

//	return color;

}

vec4 fade_color(vec4 in_color)
{
	vec4 color = in_color;
	if (darken_screen_factor > 0)
	color -= darken_screen_factor * vec4(0.8, 0.8, 0.8, 0);

	return color;
}

void main()
{
	vec2 coord = distort(texcoord);

	vec4 in_color = texture(screen_texture, coord);
	color = color_shift(in_color);
	color = fade_color(color);
}