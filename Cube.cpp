#include <glm/glm.hpp>
#include <GL/glew.h>

#include "Cube.h"



MeshData CreateCube()
{
   MeshData meshdata;

   meshdata.mScaleFactor = 0.5f;

   meshdata.mVboNormals = -1;

   using namespace glm;
   vec3 pos[8] = {vec3(-1.0f, -1.0f, -1.0f), vec3(+1.0f, -1.0f, -1.0f), vec3(+1.0f, +1.0f, -1.0f), vec3(-1.0f, +1.0f, -1.0f),
   vec3(-1.0f, -1.0f, +1.0f), vec3(+1.0f, -1.0f, +1.0f), vec3(+1.0f, +1.0f, +1.0f), vec3(-1.0f, +1.0f, +1.0f)};

   vec3 tex[8] = {vec3(-1.0f, -1.0f, -1.0f), vec3(+1.0f, -1.0f, -1.0f), vec3(+1.0f, +1.0f, -1.0f), vec3(-1.0f, +1.0f, -1.0f),
   vec3(-1.0f, -1.0f, +1.0f), vec3(+1.0f, -1.0f, +1.0f), vec3(+1.0f, +1.0f, +1.0f), vec3(-1.0f, +1.0f, +1.0f)};

   unsigned short idx[36] = { 0,2,1, 2,0,3, //bottom
                              0,5,4, 5,0,1, //front
                              1,6,5, 6,1,2, //right 
                              2,7,6, 7,2,3, //back
                              3,4,7, 4,3,0, //left
                              4,5,6, 6,7,4};//top


   //shader attrib locations
   int pos_loc = -1;
   int tex_coord_loc = -1;
   int normal_loc = -1;

   GLint program = -1;
   glGetIntegerv(GL_CURRENT_PROGRAM, &program);

   pos_loc = glGetAttribLocation(program, "pos_attrib");
   tex_coord_loc = glGetAttribLocation(program, "tex_coord_attrib");
   normal_loc = glGetAttribLocation(program, "normal_attrib");

   meshdata.mNumIndices = 36;

   glGenVertexArrays(1, &meshdata.mVao);
	glBindVertexArray(meshdata.mVao);

   //Buffer indices
   glGenBuffers(1, &meshdata.mIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshdata.mIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);

	//Buffer vertices
	glGenBuffers(1, &meshdata.mVboVerts);
	glBindBuffer(GL_ARRAY_BUFFER, meshdata.mVboVerts);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);
	glEnableVertexAttribArray(pos_loc);
	glVertexAttribPointer(pos_loc, 3, GL_FLOAT, 0, 0, 0);
		

   // buffer for vertex texture coordinates
	glGenBuffers(1, &meshdata.mVboTexCoords);
	glBindBuffer(GL_ARRAY_BUFFER, meshdata.mVboTexCoords);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tex), tex, GL_STATIC_DRAW);
	glEnableVertexAttribArray(tex_coord_loc);
	glVertexAttribPointer(tex_coord_loc, 3, GL_FLOAT, 0, 0, 0);

   // no normals
   meshdata.mVboNormals = -1;
   glDisableVertexAttribArray(normal_loc);
      
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

   return meshdata;
}

GLuint create_cube_vao()
{
	GLuint vao, vbo;
	const float cube_verts[] = { -1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f };

	//generate vao id to hold the mapping from attrib variables in shader to memory locations in vbo
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo); // Generate vbo to hold vertex attributes for triangle

						   //binding vao means that bindbuffer, enablevertexattribarray and vertexattribpointer 
						   // state will be remembered by vao
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo); //specify the buffer where vertex attribute data is stored
										//upload from main memory to gpu memory
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_verts), &cube_verts[0], GL_STATIC_DRAW);

	//get the current shader program
	GLint shader_program = -1;
	glGetIntegerv(GL_CURRENT_PROGRAM, &shader_program);

	//get a reference to an attrib variable name in a shader
	GLint pos_loc = glGetAttribLocation(shader_program, "pos_attrib");

	glEnableVertexAttribArray(pos_loc); //enable this attribute

										//tell opengl how to get the attribute values out of the vbo (stride and offset)
	glVertexAttribPointer(pos_loc, 3, GL_FLOAT, false, 0, 0);
	glBindVertexArray(0); //unbind the vao

	return vao;
}

void draw_cube_vao(GLuint vao)
{
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
