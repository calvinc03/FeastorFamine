#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;

uniform bool hit_bool;
// Output color
layout(location = 0) out  vec4 color;

void main()
{
	vec4 tex_col = texture(sampler0, vec2(texcoord.x, texcoord.y));
	if(hit_bool){
		if( tex_col.w > 0.0){
			color = vec4(0.7,0.1,0.1,1.0);
		} else {
			color = vec4(0.0,0.0,0.0,0.0);
		}
	} else {
		color = vec4(fcolor, 1.0) * tex_col;
	}
	
	
}
