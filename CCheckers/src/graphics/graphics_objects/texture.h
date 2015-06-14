#ifndef texture_h__
#define texture_h__

#include <SDL.h>
#include <SDL_surface.h>

#include <gl/glew.h>
#include <SDL_opengl.h>
#include <gl/glu.h>

class Graphics;

class Texture {
private:
	GLuint textureID;

public:
	Texture(SDL_Surface* textureSource);

	GLuint getTextureId() const;
	~Texture();
};
#endif // texture_h__
