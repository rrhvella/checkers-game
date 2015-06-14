#ifndef model_h__
#define model_h__

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <graphics/graphics_objects/mesh.h>
#include <graphics/graphics_objects/material.h>
#include <graphics/graphics_objects/program.h>

using namespace std;
using namespace glm;

class IModel {
public:
	virtual void setScale(const vec3& scaleInput) = 0;
	virtual void setTranslation(const vec3& translationInput) = 0;

	virtual void draw(const mat4& viewProjection, const LightInfo& ambientLight, const DirectionalLightInfo& diffuseLight) = 0;
	virtual void setMesh(const weak_ptr<Mesh>& mesh) = 0;
};


template <class MaterialType>
class Model : public IModel {
private:
	mat4 scaleMatrix;
	mat4 rotationMatrix;
	mat4 translationMatrix;
	mat4 transformationMatrix;

	weak_ptr<Mesh> mesh;
	MaterialType material;
	weak_ptr<Program<MaterialType>> program;

public:
	Model(const Model<MaterialType>& otherModel);
	Model(const weak_ptr<Mesh>& mesh, const MaterialType& material, const weak_ptr<Program<MaterialType>>& program);

	void setScale(const vec3& scaleInput);
	void setTranslation(const vec3& translationInput);
	void setRotation(const float angle, const vec3& axis);
	void setMaterial(const MaterialType& material);
	void setMesh(const weak_ptr<Mesh>& mesh);

	void draw(const mat4& viewProjection, const LightInfo& ambientLight, const DirectionalLightInfo& diffuseLight);
};

template <class MaterialType>
Model<MaterialType>::Model(const weak_ptr<Mesh>& mesh, const MaterialType& material, const weak_ptr<Program<MaterialType>>& program) {
	this->mesh = mesh;
	this->material = material;
	this->program = program;

	this->scaleMatrix = mat4(1.0f);
	this->rotationMatrix = mat4(1.0f);
	this->transformationMatrix = mat4(1.0f);
}

template <class MaterialType>
Model<MaterialType>::Model(const Model<MaterialType>& otherModel) {
	this->mesh = otherModel.mesh;
	this->material = otherModel.material;
	this->program = otherModel.program;

	this->scaleMatrix = otherModel.scaleMatrix;
	this->transformationMatrix = otherModel.transformationMatrix;
}

template <class MaterialType>
void Model<MaterialType>::setScale(const vec3& scaleInput) {
	scaleMatrix = scale(scaleInput);
	transformationMatrix = translationMatrix * rotationMatrix * scaleMatrix;
}

template <class MaterialType>
void Model<MaterialType>::setTranslation(const vec3& translationInput) {
	translationMatrix = translate(translationInput);
	transformationMatrix = translationMatrix * rotationMatrix * scaleMatrix;
}

template <class MaterialType>
void Model<MaterialType>::setRotation(const float angle, const vec3& axis) {
	rotationMatrix = rotate(angle, axis);
	transformationMatrix = translationMatrix * rotationMatrix * scaleMatrix;
}

template <class MaterialType>
void Model<MaterialType>::setMaterial(const MaterialType& material) {
	this->material = material;
}

template <class MaterialType>
void Model<MaterialType>::setMesh(const weak_ptr<Mesh>& mesh) {
	this->mesh = mesh;
}

template <class MaterialType>
void Model<MaterialType>::draw(const mat4& viewProjection, const LightInfo& ambientLight, const DirectionalLightInfo& diffuseLight) {
	if (auto sharedProgram = program.lock())  {
		if (auto sharedMesh = mesh.lock()) {
			sharedProgram->draw(sharedMesh, viewProjection*transformationMatrix, mat3(viewProjection*rotationMatrix), material, ambientLight, diffuseLight);
		}
	}
}

#endif // model_h__