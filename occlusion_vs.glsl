#version 430            

layout(std140, binding = 5) uniform PVM_light_uniforms
{
	mat4 M;	
	mat4 PV;	

	vec4 La;	
	vec4 Ld;	
	vec4 Ls;	
	vec4 ka;	
	vec4 kd;	
	vec4 ks;	
	vec4 pos;	
	vec4 light_pos;

	vec4 color_occ;
	vec4 color_mesh;

	float shininess;
	int pass;
};

in vec3 pos_attrib;
in vec2 tex_coord_attrib;
in vec3 normal_attrib; 

out vec2 tex_coord; 
out vec3 normal;
out vec3 light;

out float poss;

void main(void)
{
	if(pass==2)
	{
		vec3 offset = vec3(0.0);
		for(int i=0; i<1000; i++)
		{
			offset += 0.00004*sin(10.0*pos_attrib.z)*normal_attrib;
		}

		gl_Position = PV*M*vec4(pos_attrib+offset, 1.0);
		poss = 1;
	}
	else
	{
		gl_Position = PV*M*vec4(pos_attrib, 1.0);
		poss = 0;
	}
	
	tex_coord = tex_coord_attrib;
	normal = vec3(M * vec4(normal_attrib, 0.0));
	light = vec3(light_pos - M * vec4(pos_attrib, 1.0));
}