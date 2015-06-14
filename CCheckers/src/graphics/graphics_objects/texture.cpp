#include "texture.h"

Texture::Texture(SDL_Surface* textureSource) {
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureSource->w, textureSource->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureSource->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

GLuint Texture::getTextureId() const {
	return textureID;
}

Texture::~Texture() {
	glDeleteTextures(1, &textureID);
}