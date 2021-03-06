#version 430            

layout(location = 0) uniform mat4 PVM;
layout(location = 1) uniform float time;
layout(location = 2) uniform float slider = 0.5;

layout(location = 0) in vec3 pos_attrib;
layout(location = 1) in vec3 vel_attrib;
layout(location = 2) in float age_attrib;

out vec3 pos_out; 
out vec3 vel_out; 
out float age_out;

vec3 vel(vec3 p);
float rand(vec2 co);

void main(void)
{
	//Draw current particles
	gl_Position = PVM*vec4(pos_attrib, 1.0);
	gl_PointSize = mix(4.0, 40.0*slider, age_attrib/1000.0);

	//Compute particle attributes for next frame
	vel_out = vel(pos_attrib);
	pos_out = pos_attrib + 0.001*vel_out;
	age_out = age_attrib - 1.0;

	//Reinitialize particles
	if(age_out <= 0.0 || length(pos_out) > 5.0f)
	{
		vec2 seed = 10*vec2(float(gl_VertexID), time); //seed for the random number generator
		pos_out = pos_attrib*0.0003 + 3.0*(vec3(rand(seed.xy), rand(seed.yx), rand(seed.xx)) - vec3(0.5));
		age_out = 50.0 + 200.0*(rand(pos_attrib.xy+seed)-0.5);
	}
}

//Basic velocity field
vec3 v0(vec3 p);

//Implements a fractal sum to make our velocity field a little more interesting
vec3 vel(vec3 p)
{
	const int n = 6;
	vec3 octaves = vec3(0.0);
	float scale = 1.0;
	for(int i=0; i<n; i++)
	{
		octaves = octaves + 2.0*v0(scale*p)/scale;
		scale*= 2.0;
	}
	return octaves;
}

vec3 v0(vec3 p)
{
	return 1.0*vec3(sin(p.y*20.0+time-10.0));//, -sin(p.x*10.0+9.0*time+10.0), +cos(p.z+0.1*time));
}

// a hacky random numver generator
float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}
