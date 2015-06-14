#ifndef program_h__
#define program_h__

#include <string>
#include <sstream>

#include <gl/glew.h>
#include <SDL_opengl.h>
#include <gl/glu.h>

#include <graphics/graphics_objects/mesh.h>
#include <graphics/graphics_objects/light.h>
#include <graphics/graphics_objects/material.h>

using namespace std;

template <class MaterialType>
class Program {
private:
	GLint vsVertexLocation;
	GLint vsNormalLocation;

	GLint vsTextureCoordinatesLocation;

	GLint vsModelViewProjectionLocation;
	GLint vsNormalMatrixLocation;

	GLint fsAmbientLightColorLocation;
	GLint fsDiffuseLightColorLocation;

	GLint fsDiffuseLightDirectionLocation;

	GLint fsAmbientIntensityLocation;
	GLint fsDiffuseIntensityLocation;

	GLuint initialiseFragmentShader(const string& fragmentShaderSource);

	GLuint initialiseVertexShader(const string& vertexShaderSource);

	void initialiseAttributes();

protected:
	virtual void enableVariables() = 0;
	virtual void setValues(const MaterialType& material) = 0;
	virtual void disableVariables() = 0;

	GLuint glProgramID;

public:
	Program(const string& vertexShaderSource, const string& fragmentShaderSource);

	void draw(const shared_ptr<Mesh>& mesh, const mat4& transformationMatrix, const mat3& normalMatrix, const MaterialType& material, const LightInfo& ambientLight, const DirectionalLightInfo& diffuseLight);

	~Program();
};

class SolidColoredProgram : public Program<SolidColoredMaterial> {
protected:
	void enableVariables() {
	}

	void setValues(const SolidColoredMaterial& material);

	void disableVariables() {
	}

private:
	GLint fsColorLocation;

public:
	SolidColoredProgram(const string& vertexShaderSource, const string& fragmentShaderSource);
};

class TexturedProgram : public Program<TexturedMaterial> {
protected:
	void enableVariables() {
	}

	void setValues(const TexturedMaterial& material);

	void disableVariables() {
	}
private:
	GLint fsTextureSamplerLocation;

public:
	TexturedProgram(const string& vertexShaderSource, const string& fragmentShaderSource);
};


template <class MaterialType>
GLuint Program<MaterialType>::initialiseFragmentShader(const string& fragmentShaderSource) {
	//Create fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	if (fragmentShader == 0) {
		throw exception((const char*)glewGetErrorString(glGetError()));
	}

	//Set fragment source
	const char* data = fragmentShaderSource.c_str();
	glShaderSource(fragmentShader, 1, &data, NULL);

	//Compile fragment source
	glCompileShader(fragmentShader);

	//Check fragment shader for errors
	GLint fShaderCompiled = GL_FALSE;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled);

	if (fShaderCompiled != GL_TRUE) {
		GLint maxLength = 0;
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

		GLchar* infoLog = new GLchar[maxLength];
		glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, infoLog);

		stringstream errorStream;

		errorStream << "Failed to load fragment shader, please see line below for error: " << endl << endl;
		errorStream << infoLog;

		delete[] infoLog;

		throw exception(errorStream.str().c_str());
	}

	glAttachShader(glProgramID, fragmentShader);

	return fragmentShader;
}

template <class MaterialType>
GLuint Program<MaterialType>::initialiseVertexShader(const string& vertexShaderSource) {
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	if (vertexShader == 0) {
		throw exception((const char*)glewGetErrorString(glGetError()));
	}

	//Set vertex source
	const char* data = vertexShaderSource.c_str();
	glShaderSource(vertexShader, 1, &data, NULL);

	//Compile vertex source
	glCompileShader(vertexShader);

	//Check vertex shader for errors
	GLint vShaderCompiled = GL_FALSE;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vShaderCompiled);

	if (vShaderCompiled != GL_TRUE) {
		GLint maxLength = 0;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

		GLchar* infoLog = new GLchar[maxLength];
		glGetShaderInfoLog(vertexShader, maxLength, &maxLength, infoLog);

		stringstream errorStream;

		errorStream << "Failed to load vertex shader, please see line below for error: " << endl << endl;
		errorStream << infoLog;

		delete[] infoLog;

		throw exception(errorStream.str().c_str());
	}

	glAttachShader(glProgramID, vertexShader);

	return vertexShader;
}

template <class MaterialType>
void Program<MaterialType>::initialiseAttributes() {
	vsTextureCoordinatesLocation = glGetAttribLocation(glProgramID, "textureCoords");
	if (vsTextureCoordinatesLocation == -1) {
		//Texture coordinates are optional
	}

	vsVertexLocation = glGetAttribLocation(glProgramID, "modelVertex");
	if (vsVertexLocation == -1) {
		throw exception((const char*)glewGetErrorString(glGetError()));
	}

	vsNormalLocation = glGetAttribLocation(glProgramID, "modelNormal");
	if (vsNormalLocation == -1) {
		throw exception((const char*)glewGetErrorString(glGetError()));
	}

	vsModelViewProjectionLocation = glGetUniformLocation(glProgramID, "modelViewProjection");
	if (vsModelViewProjectionLocation == -1) {
		throw exception((const char*)glewGetErrorString(glGetError()));
	}

	vsNormalMatrixLocation = glGetUniformLocation(glProgramID, "normalMatrix");
	if (vsNormalMatrixLocation == -1) {
		throw exception((const char*)glewGetErrorString(glGetError()));
	}

	fsAmbientLightColorLocation = glGetUniformLocation(glProgramID, "ambientLightColor");
	if (fsAmbientLightColorLocation == -1) {
		throw exception((const char*)glewGetErrorString(glGetError()));
	}

	fsDiffuseLightColorLocation = glGetUniformLocation(glProgramID, "diffuseLightColor");
	if (fsDiffuseLightColorLocation == -1) {
		throw exception((const char*)glewGetErrorString(glGetError()));
	}

	fsDiffuseLightDirectionLocation = glGetUniformLocation(glProgramID, "diffuseLightDirection");
	if (fsDiffuseLightDirectionLocation == -1) {
		throw exception((const char*)glewGetErrorString(glGetError()));
	}

	fsAmbientIntensityLocation = glGetUniformLocation(glProgramID, "ambientIntensity");
	if (fsAmbientIntensityLocation == -1) {
		throw exception((const char*)glewGetErrorString(glGetError()));
	}

	fsDiffuseIntensityLocation = glGetUniformLocation(glProgramID, "diffuseIntensity");
	if (fsDiffuseIntensityLocation == -1) {
		throw exception((const char*)glewGetErrorString(glGetError()));
	}
}

template <class MaterialType>
Program<MaterialType>::Program(const string& vertexShaderSource, const string& fragmentShaderSource) {
	glProgramID = glCreateProgram();

	if (glProgramID == 0) {
		throw exception((const char*)glewGetErrorString(glGetError()));
	}

	GLuint vertexShader = initialiseVertexShader(vertexShaderSource);
	GLuint fragmentShader = initialiseFragmentShader(fragmentShaderSource);

	glLinkProgram(glProgramID);

	glDetachShader(glProgramID, vertexShader);
	glDeleteShader(vertexShader);

	glDetachShader(glProgramID, fragmentShader);
	glDeleteShader(fragmentShader);

	GLint programSuccess = GL_TRUE;
	glGetProgramiv(glProgramID, GL_LINK_STATUS, &programSuccess);

	if (programSuccess != GL_TRUE) {
		throw exception((const char*)glewGetErrorString(glGetError()));
	}

	initialiseAttributes();
}

template <class MaterialType>
void Program<MaterialType>::draw(const shared_ptr<Mesh>& mesh, const mat4& transformationMatrix, const mat3& normalMatrix, const MaterialType& material, const LightInfo& ambientLight, const DirectionalLightInfo& diffuseLight) {
	glUseProgram(glProgramID);

	//Enable vertex position
	glEnableVertexAttribArray(vsVertexLocation);
	glEnableVertexAttribArray(vsNormalLocation);
	glEnableVertexAttribArray(vsTextureCoordinatesLocation);
	enableVariables();

	setValues(material);

	//Set mvp data
	glUniformMatrix4fv(vsModelViewProjectionLocation, 1, GL_FALSE, &transformationMatrix[0][0]);

	//Set normal matrix
	glUniformMatrix3fv(vsNormalMatrixLocation, 1, GL_FALSE, &normalMatrix[0][0]);

	//Set ambient light info
	glUniform3fv(fsAmbientLightColorLocation, 1, &ambientLight.color[0]);
	glUniform1f(fsAmbientIntensityLocation, ambientLight.intensity);

	//Set directional light info
	glUniform3fv(fsDiffuseLightColorLocation, 1, &diffuseLight.color[0]);
	glUniform3fv(fsDiffuseLightDirectionLocation, 1, &diffuseLight.direction[0]);
	glUniform1f(fsDiffuseIntensityLocation, diffuseLight.intensity);

	//Bind model vertices
	glBindBuffer(GL_ARRAY_BUFFER, mesh->getVertexData());

	//Set vertex attribute
	glVertexAttribPointer(vsVertexLocation, mesh->getVertexSize(), GL_FLOAT, GL_FALSE, 0, (void*)0);

	//Bind model normals
	glBindBuffer(GL_ARRAY_BUFFER, mesh->getNormalData());

	//Set normal attribute
	glVertexAttribPointer(vsNormalLocation, mesh->getVertexSize(), GL_FLOAT, GL_FALSE, 0, (void*)0);

	//Bind model texture coordinates
	glBindBuffer(GL_ARRAY_BUFFER, mesh->getTextureCoordinates());

	//Set texture coordinate attribute
	glVertexAttribPointer(vsTextureCoordinatesLocation, mesh->getTextureCoordinateSize(), GL_FLOAT, GL_FALSE, 0, (void*)0);

	//Bind indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->getIndices());

	//Set index data and render
	glDrawElements(GL_TRIANGLES, mesh->getNumberOfIndices(), GL_UNSIGNED_INT, (void*)0);

	//Disable vertex position
	glDisableVertexAttribArray(vsVertexLocation);
	glDisableVertexAttribArray(vsNormalLocation);
	glDisableVertexAttribArray(vsTextureCoordinatesLocation);
	disableVariables();

	//Unbind program
	glUseProgram(NULL);
}

template <class MaterialType>
Program<MaterialType>::~Program() {
	glDeleteProgram(glProgramID);
}

#endif // program_h__
