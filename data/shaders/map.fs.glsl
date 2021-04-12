#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform int highlight;
uniform int season;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	if (highlight == 0)
	{
		//color = vec4(1.0, 0.0, 0.0, 1.0);
		color = vec4(fcolor,1) * texture(sampler0, vec2(texcoord.x, texcoord.y));

	}
	else
	{
		//color = vec4(1.0, 1.0,  0.0, 1.0);
		color = vec4(fcolor, 0.7) * texture(sampler0, vec2(texcoord.x, texcoord.y));
		color += vec4(0.4, 0.3, 0.0, 0.3);
	}

	// Make the season's adjustments
	if (season == 0) {
	// TODO: season always 0
	}
	else if (season == 1) {
		color += vec4(0.2, 0.0, 0.0, 0.0);
	}
	else if (season == 2) {
		color += vec4(0.4, 0.0, 0.0, 0.0);
	}
	else if (season == 3) {
		color += vec4(0.0, 0.0, 0.5, 0.0);
	}

//	// ref: https://stackoverflow.com/questions/9234724/how-to-change-hue-of-a-texture-with-glsl
//	const vec4  kRGBToYPrime = vec4 (0.299, 0.587, 0.114, 0.0);
//    const vec4  kRGBToI     = vec4 (0.596, -0.275, -0.321, 0.0);
//    const vec4  kRGBToQ     = vec4 (0.212, -0.523, 0.311, 0.0);
//
//    const vec4  kYIQToR   = vec4 (1.0, 0.956, 0.621, 0.0);
//    const vec4  kYIQToG   = vec4 (1.0, -0.272, -0.647, 0.0);
//    const vec4  kYIQToB   = vec4 (1.0, -1.107, 1.704, 0.0);
//
//    // Convert to YIQ
//    float   YPrime  = dot (color, kRGBToYPrime);
//    float   I      = dot (color, kRGBToI);
//    float   Q      = dot (color, kRGBToQ);
//
//    // Calculate the hue and chroma
//    float   hue     = atan (Q, I);
//    float   chroma  = sqrt (I * I + Q * Q);
//
//    // Make the season's adjustments
//	if (season == 0) {
//	hue += 0.0;
//	}
//	else if (season == 1) {
//	hue += -30.0;
//	}
//	else if (season == 2) {
//	hue += -60.0;
//	}
//	else if (season == 3) {
//	hue += 100.0;
//	}
//
//	// Convert back to YIQ
//    Q = chroma * sin (hue);
//    I = chroma * cos (hue);
//
//    // Convert back to RGB
//    vec4    yIQ   = vec4 (YPrime, I, Q, 0.0);
//    color.r = dot (yIQ, kYIQToR);
//    color.g = dot (yIQ, kYIQToG);
//    color.b = dot (yIQ, kYIQToB);
}
