#include "program.h"

void SolidColoredProgram::setValues(const SolidColoredMaterial& material) {
	glUniform4fv(fsColorLocation, 1, &(material.color[0]));
}

SolidColoredProgram::SolidColoredProgram(const string& vertexShaderSource, const string& fragmentShaderSource) : Program(vertexShaderSource, fragmentShaderSource) {
	fsColorLocation = -1;

	fsColorLocation = glGetUniformLocation(glProgramID, "color");
	if (fsColorLocation == -1) {
		throw exception((const char*)glewGetErrorString(glGetError()));
	}
}

void TexturedProgram::setValues(const TexturedMaterial& material) {
	//Set texture
	if (auto sharedTexture = material.texture.lock()){
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sharedTexture->getTextureId());
		glUniform1i(fsTextureSamplerLocation, 0);
	}
}

TexturedProgram::TexturedProgram(const string& vertexShaderSource, const string& fragmentShaderSource) : Program(vertexShaderSource, fragmentShaderSource) {
	fsTextureSamplerLocation = -1;

	fsTextureSamplerLocation = glGetUniformLocation(glProgramID, "textureSampler");
	if (fsTextureSamplerLocation == -1) {
		throw exception((const char*)glewGetErrorString(glGetError()));
	}
}
