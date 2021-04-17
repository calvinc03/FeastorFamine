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

void main()
{
	color = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
	color += 0.3 * vec4(0.f, sin(0.1 * time), cos(0.1 * time), 0.f);
}
