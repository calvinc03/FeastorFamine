#version 330

// From vertex shader
in vec2 texcoord;
uniform float time;
// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;

uniform int highlight;

// Output color
layout(location = 0) out  vec4 color;

vec2 distort(vec2 uv)
{
    vec2 coord = uv.xy;
    coord += 0.004*vec2(sin(0.05*time + 1.*uv.y), cos(1234. + 0.025*time + 2.*uv.x));
    coord += 0.002*vec2(sin(10.*(0.05*time + 1.*uv.y)), cos(10.*(1234. + 0.025*time + 2.*uv.x)));
    return coord;
}

void main()
{
    vec2 coord = distort(texcoord);
	color = vec4(fcolor, cos(time)) * texture(sampler0, vec2(coord.x, coord.y));
}
