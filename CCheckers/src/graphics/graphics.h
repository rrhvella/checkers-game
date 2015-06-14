#ifndef graphics_h__
#define graphics_h__

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_surface.h>

#include <gl/glew.h>
#include <SDL_opengl.h>
#include <gl/glu.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp> 

#include <iostream>
#include <fstream>
#include <memory>
#include <sstream>
#include <set>

#include <vector>

#include <graphics/model.h>

#include <graphics/graphics_objects/program.h>
#include <graphics/graphics_objects/light.h>
#include <graphics/graphics_objects/material.h>
#include <graphics/graphics_objects/texture.h>

#include <math/ray.h>

#include <graphics/mesh_data/plane.h>

using namespace glm;
using namespace std;

class Graphics
{

private:
	set<shared_ptr<Texture>> textures;
	set<shared_ptr<IModel>> models;
	set<shared_ptr<Mesh>> meshes;

	SDL_Window* gameWindow;
	GLuint glProgramID;
	SDL_GLContext glContext;

	vec3 eye;
	mat4 viewMatrix;

	mat4 projectionMatrix;

	mat4 viewProjectionMatrix;

	LightInfo ambientLight;
	DirectionalLightInfo diffuseLight;

	weak_ptr<Mesh> planeMesh;

	shared_ptr<SolidColoredProgram> solidColoredProgram;
	shared_ptr<TexturedProgram> texturedProgram;

	void initialiseSDL(const string& windowTitle, int windowWidth, int windowHeight);
	void initialiseOpenGL(const vec4& clearColor);

	static string readFileContents(const string& fileName);
	static void logError(const char* error);

	void initialiseMeshes();

	template <class T>
	shared_ptr<T> loadProgram(const string& vertexShaderPath, const string& fragmentShaderPath);

public:
	Graphics(const string& windowTitle, int windowWidth, int windowHeight, const vec4& clearColor);

	void draw();

	weak_ptr<Mesh> createMesh(const GLfloat* modelRawVertexData, const GLfloat* modelRawNormalData, GLuint vertexSize, GLuint textureCoordinateSize, const GLfloat* modelRawTextureCoordinates, const GLuint* rawIndices, GLuint numberOfVertices, GLuint numberOfIndices);

	template <class MaterialType>
	shared_ptr<Model<MaterialType>> createModel(const weak_ptr<Mesh>& mesh, const MaterialType& material, const weak_ptr<Program<MaterialType>>& program);

	template <class MaterialType>
	shared_ptr<Model<MaterialType>> createModel(const shared_ptr<Model<MaterialType>>& model);

	void removeModel(const shared_ptr<IModel>& modelToRemove);
	
	weak_ptr<Texture> loadTexture(const string& texturePath);

	void setView(const vec3& eye, const vec3& center, const vec3& up);
	void setProjection(float fovy, float zNear, float zFar);

	void setAmbientLight(const LightInfo& ambientLight);
	void setDiffuseLight(const DirectionalLightInfo& diffuseLight);

	Ray screenCoordinateToRay(const vec2& screenCoordinates);

	const weak_ptr<Mesh> getPlaneMesh() const;

	const weak_ptr<TexturedProgram> getTexturedProgram();
	const weak_ptr<SolidColoredProgram> getSolidColoredProgram();

	~Graphics();
};

template <class MaterialType>
shared_ptr<Model<MaterialType>> Graphics::createModel(const weak_ptr<Mesh>& mesh, const MaterialType& material, const weak_ptr<Program<MaterialType>>& program) {
	const shared_ptr<Model<MaterialType>>& model = make_shared<Model<MaterialType>>(mesh, material, program);
	models.insert(model);

	return model;
}

template <class MaterialType>
shared_ptr<Model<MaterialType>> Graphics::createModel(const shared_ptr<Model<MaterialType>>& modelToCopy) {
	const shared_ptr<Model<MaterialType>>& model = make_shared<Model<MaterialType>>(*modelToCopy);
	models.insert(model);

	return model;
}

#endif // graphics_h__
