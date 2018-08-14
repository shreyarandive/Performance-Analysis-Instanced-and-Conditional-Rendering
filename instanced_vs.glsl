#version 430            

uniform mat4 PVM;
in vec3 pos_attrib;
in vec2 tex_coord_attrib;
in vec3 normal_attrib;

layout (location = 3) uniform bool instanced;
layout (location = 4) uniform mat4 pos_not_instanced;
in mat4 pos_instanced;

out vec2 tex_coord;  

void main(void)
{
	if(instanced)
	{
		gl_Position = PVM * pos_instanced * vec4(pos_attrib, 1.0);
		tex_coord = tex_coord_attrib;
	}


	else
	{
		gl_Position = PVM * pos_not_instanced * vec4(pos_attrib, 1.0);
		tex_coord = tex_coord_attrib;
	}

}