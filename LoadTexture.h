#ifndef __LOADTEXTURE_H__
#define __LOADTEXTURE_H__

#include <string>
#include <windows.h>
#include "GL/glew.h"
#include "GL/gl.h"

GLuint LoadTexture(const char* fname);
GLuint LoadCube(const std::string& fname);

#endif