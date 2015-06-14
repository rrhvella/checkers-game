#ifndef mesh_h__
#define mesh_h__

#include <gl/glew.h>
#include <SDL_opengl.h>
#include <gl/glu.h>

class Mesh {
private:
	GLuint normalData;
	GLuint vertexData;
	GLuint vertexSize;

	GLuint textureCoordinates;
	GLuint textureCoordinateSize;

	GLuint numberOfVertices;

	GLuint numberOfIndices;

	GLuint indices;

public:
	Mesh(const GLfloat* modelRawVertexData, const GLfloat* modelRawNormalData, GLuint vertexSize, GLuint textureCoordinateSize, const GLfloat* modelRawTextureCoordinates, const GLuint* rawIndices, GLuint numberOfVertices, GLuint numberOfIndices);

	GLuint getVertexData() const;
	GLuint getNormalData() const;
	GLuint getTextureCoordinates() const;
	GLuint getIndices() const;

	GLuint getVertexSize() const;
	GLuint getTextureCoordinateSize() const;

	GLuint getNumberOfVertices() const;
	GLuint getNumberOfIndices() const;

	~Mesh();
};

#endif // mesh_h__
