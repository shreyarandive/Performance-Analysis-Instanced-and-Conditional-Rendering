#ifndef __CUBE_H__
#define __CUBE_H__

#include "LoadMesh.h"
#include <windows.h>
#include <GL/glew.h>

MeshData CreateCube();

GLuint create_cube_vao();
void draw_cube_vao(GLuint vao);

#endif