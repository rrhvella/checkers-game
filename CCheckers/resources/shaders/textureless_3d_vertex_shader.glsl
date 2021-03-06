#version 330 core
 
uniform mat4 modelViewProjection;
uniform mat3 normalMatrix;

in vec3 modelVertex;
in vec3 modelNormal;

out vec3 normal;

void main() { 
	gl_Position = modelViewProjection * vec4(modelVertex, 1.0); 
	normal = normalMatrix * modelNormal; 
}
