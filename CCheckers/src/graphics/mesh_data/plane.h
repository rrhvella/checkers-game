#ifndef plane_h__
#define plane_h__

#include <gl/glew.h>
#include <SDL_opengl.h>
#include <gl/glu.h>

const GLfloat PLANE_VERTICES[] = {
	-0.5f, 0.0f, -0.5f,
	0.5f, 0.0f, -0.5f,
	0.5f, 0.0f, 0.5f,
	-0.5f, 0.0f, 0.5f,
};

const GLfloat PLANE_TEXTURE_COORDINATES[] = {
	0.0f, 0.0f,
	1.0f, 0.0f, 
	1.0f, 1.0f, 
	0.0f, 1.0f, 
};

const GLfloat PLANE_NORMALS[] = {
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
};

const GLuint PLANE_INDICES[] = {
	0, 
	1,
	2,
	2,
	3,
	0
};

const GLuint PLANE_VERTEX_SIZE = 3;

const GLuint PLANE_TEXTURE_COORDINATE_SIZE = 2;

const unsigned int PLANE_NUMBER_OF_VERTICES = 4;

const unsigned int PLANE_NUMBER_OF_INDICES = 2 * 3;
#endif // plane_h__