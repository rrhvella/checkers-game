#include "graphics.h"

void Graphics::logError(const char* error) {
	cout << error;
}

void Graphics::initialiseSDL(const string& windowTitle, int windowWidth, int windowHeight) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		throw exception(SDL_GetError());
	}

	if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
		throw exception(IMG_GetError());
	}

	gameWindow = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	
	if (gameWindow == nullptr) {
		throw exception(SDL_GetError());
	}
}

void Graphics::initialiseOpenGL(const vec4& clearColor) {
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	glContext = SDL_GL_CreateContext(gameWindow);
	if (glContext == nullptr) {
		throw exception(SDL_GetError());
	}

	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		throw exception((const char*)glewGetErrorString(glewError));
	}

	glEnable(GL_TEXTURE_2D);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.5);

	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);

	solidColoredProgram = loadProgram<SolidColoredProgram>("resources/shaders/textureless_3d_vertex_shader.glsl", "resources/shaders/solid_coloured_fragment_shader.glsl");
	texturedProgram = loadProgram<TexturedProgram>("resources/shaders/textured_3d_vertex_shader.glsl", "resources/shaders/textured_fragment_shader.glsl");
}


void Graphics::removeModel(const shared_ptr<IModel>& modelToRemove) {
	models.erase(modelToRemove);
}

std::string Graphics::readFileContents(const string& fileName) {
	try	{
		ifstream fileStream(fileName, ios::in);
	
		if (fileStream.is_open()) {
			stringstream stringStream;
			stringStream << fileStream.rdbuf();
	
			return stringStream.str();
		} else {
			logError("Unable to open file");
			return "";
		}
	}
	catch (exception& e) {
		logError(e.what());
		return "";
	}
}

Graphics::Graphics(const string& windowTitle, int windowWidth, int windowHeight, const vec4& clearColor) {
	viewProjectionMatrix = mat4(1.0f);
	
	viewMatrix = mat4(1.0f);
	projectionMatrix = mat4(1.0f);
	
	initialiseSDL(windowTitle, windowWidth, windowHeight);
	initialiseOpenGL(clearColor);
	initialiseMeshes();
};

Graphics::~Graphics() {
	if (glProgramID != 0) {
		glDeleteProgram(glProgramID);
	}

	if (glContext != nullptr) {
		SDL_GL_DeleteContext(glContext);
	}

	if (gameWindow != nullptr){
		SDL_DestroyWindow(gameWindow);
	}

	textures.clear();
	models.clear();
	meshes.clear();

	solidColoredProgram.reset();
	texturedProgram.reset();

	SDL_Quit();
}

void Graphics::draw() {	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (set<shared_ptr<IModel>>::iterator it = models.begin(); it != models.end(); it++) {
		(*it)->draw(viewProjectionMatrix, ambientLight, diffuseLight);
	}

	SDL_GL_SwapWindow(gameWindow);
}

weak_ptr<Mesh> Graphics::createMesh(const GLfloat* modelRawVertexData, const GLfloat* modelRawNormalData, GLuint vertexSize, GLuint textureCoordinateSize, const GLfloat* modelRawTextureCoordinates, const GLuint* rawIndices, GLuint numberOfVertices, GLuint numberOfIndices) {
	const shared_ptr<Mesh>& mesh = make_shared<Mesh>(modelRawVertexData, modelRawNormalData, vertexSize, textureCoordinateSize, modelRawTextureCoordinates, rawIndices, numberOfVertices, numberOfIndices);
	meshes.insert(mesh);

	return mesh;
}

void Graphics::setView(const vec3& eye, const vec3& center, const vec3& up) {
	this->eye = eye;

	viewMatrix = lookAt(eye, center, up);

	viewProjectionMatrix = projectionMatrix * viewMatrix;
}

Ray Graphics::screenCoordinateToRay(const vec2& screenCoordinates) {
	int width;
	int height;

	SDL_GetWindowSize(gameWindow, &width, &height);

	vec3 worldMouseCoordinates = unProject(vec3(screenCoordinates.x, height - screenCoordinates.y, 1.0f), viewMatrix, projectionMatrix, vec4(0, 0, width, height));
	vec3 rayDirection = normalize(worldMouseCoordinates - eye);

	return Ray(eye, rayDirection);
}

void Graphics::setProjection(float fovy, float zNear, float zFar) {
	int width;
	int height;

	SDL_GetWindowSize(gameWindow, &width, &height);

	projectionMatrix = perspective(fovy, (float)width / height, zNear, zFar);

	viewProjectionMatrix = projectionMatrix * viewMatrix;
}

void Graphics::setAmbientLight(const LightInfo& ambientLight) {
	this->ambientLight = ambientLight;
}

void Graphics::setDiffuseLight(const DirectionalLightInfo& diffuseLight) {
	this->diffuseLight = diffuseLight;
}

const weak_ptr<Mesh> Graphics::getPlaneMesh() const {
	return planeMesh;
}

void Graphics::initialiseMeshes() {
	planeMesh = createMesh(PLANE_VERTICES, PLANE_NORMALS, PLANE_VERTEX_SIZE, PLANE_TEXTURE_COORDINATE_SIZE, PLANE_TEXTURE_COORDINATES, PLANE_INDICES, PLANE_NUMBER_OF_VERTICES, PLANE_NUMBER_OF_INDICES);
}

weak_ptr<Texture> Graphics::loadTexture(const string& texturePath) {
	SDL_Surface* surface = IMG_Load(texturePath.c_str());
	
	if (surface == nullptr) {
		throw exception(SDL_GetError());
	}

	const shared_ptr<Texture>& texture = make_shared<Texture>(surface);
	textures.insert(texture);

	return texture;
}

const weak_ptr<TexturedProgram> Graphics::getTexturedProgram() {
	return texturedProgram;
}


const weak_ptr<SolidColoredProgram> Graphics::getSolidColoredProgram() {
	return solidColoredProgram;
}

template <class T>
shared_ptr<T> Graphics::loadProgram(const string& vertexShaderPath, const string& fragmentShaderPath) {
	string& vertexShaderFileContents = readFileContents(vertexShaderPath);
	string& fragmentShaderFileContents = readFileContents(fragmentShaderPath);

	return make_shared<T>(vertexShaderFileContents, fragmentShaderFileContents);
}