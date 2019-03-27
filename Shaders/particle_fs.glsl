#version 430

out vec4 fragcolor;           
 
in float age_out;

void main(void)
{  
	//Creat a disk by applying a smoothstep function to the radius
	float r = length(gl_PointCoord.st-vec2(0.4));
	float s = smoothstep(0.4, 0.2, r);

	vec4 color0 = vec4(0.1, 0.5, 0.3, 3.7*s);
	vec4 color1 = vec4(0.9, 0.1, 0.0, 4.3*s);
	fragcolor = mix(color1, color0, age_out/500.0);
}
/*
//Creat a disk by applying a smoothstep function to the radius
	float r = length(gl_PointCoord.st-vec2(0.5));
	if(r > 0.5) discard;

	float s = smoothstep(0.5, 0.3, r);

	vec4 color0 = vec4(0.7, 0.4, 0.3, 2.7*s);
	vec4 color1 = vec4(0.7, 0.1, 0.0, 2.3*s);
	fragcolor = mix(color1, color0, age_out/500.0);

*/



















