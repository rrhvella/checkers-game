#include "mesh.h"

Mesh::Mesh(const GLfloat* modelRawVertexData, const GLfloat* modelRawNormalData, GLuint vertexSize, GLuint textureCoordinateSize, const GLfloat* modelRawTextureCoordinates, const GLuint* rawIndices, GLuint numberOfVertices, GLuint numberOfIndices) {
	this->numberOfIndices = numberOfIndices;

	this->vertexSize = vertexSize;
	this->textureCoordinateSize = textureCoordinateSize;

	glGenBuffers(1, &vertexData);
	glBindBuffer(GL_ARRAY_BUFFER, vertexData);
	glBufferData(GL_ARRAY_BUFFER, numberOfVertices * vertexSize * sizeof(GLfloat), modelRawVertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &normalData);
	glBindBuffer(GL_ARRAY_BUFFER, normalData);
	glBufferData(GL_ARRAY_BUFFER, numberOfVertices * vertexSize * sizeof(GLfloat), modelRawNormalData, GL_STATIC_DRAW);

	glGenBuffers(1, &textureCoordinates);
	glBindBuffer(GL_ARRAY_BUFFER, textureCoordinates);
	glBufferData(GL_ARRAY_BUFFER, numberOfVertices * textureCoordinateSize * sizeof(GLfloat), modelRawTextureCoordinates, GL_STATIC_DRAW);

	glGenBuffers(1, &indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numberOfIndices * sizeof(GLuint), rawIndices, GL_STATIC_DRAW);
}

GLuint Mesh::getVertexData() const {
	return vertexData;
}

GLuint Mesh::getTextureCoordinates() const {
	return textureCoordinates;
}

GLuint Mesh::getNumberOfVertices() const {
	return numberOfVertices;
}

GLuint Mesh::getIndices() const {
	return indices;
}

GLuint Mesh::getNumberOfIndices() const {
	return numberOfIndices;
}

GLuint Mesh::getVertexSize() const {
	return vertexSize;
}

GLuint Mesh::getTextureCoordinateSize() const {
	return textureCoordinateSize;
}

GLuint Mesh::getNormalData() const {
	return normalData;
}

Mesh::~Mesh() {
	glDeleteBuffers(1, &vertexData);
	glDeleteBuffers(1, &normalData);
	glDeleteBuffers(1, &textureCoordinates);
	glDeleteBuffers(1, &indices);
}