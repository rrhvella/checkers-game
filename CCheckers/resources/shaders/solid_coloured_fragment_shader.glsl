#version 330 core

uniform vec4 color;

uniform vec3 ambientLightColor;
uniform vec3 diffuseLightColor;

uniform vec3 diffuseLightDirection;

uniform float ambientIntensity;
uniform float diffuseIntensity;

in vec3 normal;
out vec4 outputColor;

vec4 applyLight(vec4 materialColor) {
	vec4 diffuseEffect;

	vec4 ambientEffect = vec4(ambientLightColor * ambientIntensity, 1.0f);
    float diffuseFactor = dot(normal, -diffuseLightDirection);

    if (diffuseFactor > 0) {
        diffuseEffect = vec4(diffuseLightColor * diffuseIntensity * diffuseFactor, 1.0f);
    }
    else {
        diffuseEffect = vec4(0, 0, 0, 0);
    }

	return materialColor * (ambientEffect + diffuseEffect);
}

void main() { 
    outputColor = applyLight(color);
}
