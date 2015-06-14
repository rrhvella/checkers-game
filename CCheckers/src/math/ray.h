#ifndef ray_h__
#define ray_h__

#include <glm/glm.hpp>

using namespace glm;

struct Ray {
	vec3 origin;
	vec3 direction;

	Ray() {

	}

	Ray(const vec3& origin, const vec3 direction) {
		this->origin = origin;
		this->direction = direction;
	}

	bool tryIntersectWithPlane(const vec3& planeNormal, const vec3& pointOnPlane, vec3* result) {
		float t = dot((pointOnPlane - origin), planeNormal) / dot(direction, planeNormal);

		if (t < 0) {
			return false;
		}

		*result = origin + t * direction;
		
		return true;
	}
};

#endif