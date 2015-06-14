#ifndef material_h__
#define material_h__

#include <memory>
#include <glm/glm.hpp>

#include <graphics/graphics_objects/texture.h>

using namespace glm;
using namespace std;

struct TexturedMaterial {
	weak_ptr<Texture> texture;

	TexturedMaterial() {
	}

	TexturedMaterial(const weak_ptr<Texture>& texture) {
		this->texture = texture;
	}

};

struct SolidColoredMaterial {
	vec4 color;

	SolidColoredMaterial() {
	}

	SolidColoredMaterial(const vec4& color) {
		this->color = color;
	}
};

#endif // material_h__