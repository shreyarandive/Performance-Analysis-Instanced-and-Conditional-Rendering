#version 430

uniform samplerCube cubemap;

out vec4 fragcolor;           
in vec3 pos;
     
void main(void)
{   
   fragcolor = texture(cubemap, pos);
}