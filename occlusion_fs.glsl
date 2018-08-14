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

uniform sampler2D texColor; 
uniform sampler2D texColor_2;

out vec4 fragcolor;           
in vec2 tex_coord; 
in vec3 normal; 
in vec3 light; 

in float poss;

void main(void)
{ 
   if(pass==0) //occluders
   {
      vec3 n = normalize(normal);
      vec3 l = normalize(light);
      fragcolor = texture(texColor_2, tex_coord) * color_occ *(kd*abs(dot(n, l)) + ka + ks);;
      return;
   }

   if(pass==1) // bounding volumes
   {
      fragcolor = vec4(1.0, 1.0, 1.0, 1.0);
      return;
   }

   //meshes
   vec3 n = normalize(normal);
   vec3 l = normalize(light);


   fragcolor = texture(texColor, tex_coord) * color_mesh * (kd*abs(dot(n, l)) + ka + ks);
}




















