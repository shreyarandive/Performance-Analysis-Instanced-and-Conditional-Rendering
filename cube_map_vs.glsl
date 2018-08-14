#version 430         
uniform mat4 PVM_cube;

in vec3 pos_attrib; 
out vec3 pos;

void main(void)
{
   //Compute clip-space vertex position
   gl_Position = PVM_cube*vec4(pos_attrib, 1.0);     //w = 1 becase this is a point
   pos = pos_attrib;
}