#ifndef light_h__
#define light_h__

#include <glm/glm.hpp>

using namespace glm;

struct LightInfo {
	vec3 color;
	float intensity;

	LightInfo() {
		this->color = vec3(0.0f, 0.0f, 0.0f);
		this->intensity = 1.0f;
	}

	LightInfo(const vec3& color, float intensity) {
		this->color = color;
		this->intensity = intensity;
	}
};

struct DirectionalLightInfo {
	vec3 color;
	vec3 direction;
	float intensity;

	DirectionalLightInfo() {
		this->color = vec3(0.0f, 0.0f, 0.0f);
		this->direction = vec3(0.0f, 0.0f, 0.0f);
		this->intensity = 1.0f;
	}

	DirectionalLightInfo(const vec3& color, const vec3& direction, float intensity) {
		this->color = color;
		this->direction = direction;
		this->intensity = intensity;
	}
};


#endif // light_h__