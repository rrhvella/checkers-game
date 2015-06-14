#ifndef checkers_game_h__
#define checkers_game_h__

#include <glm/gtc/constants.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <vector>
#include <set>

#include <checkers/checkers_model.h>
#include <checkers/checkers_ai.h>

#include <graphics/model.h>
#include <graphics/graphics.h>


using namespace glm;
using namespace std;

class CheckersGame {
private:
	enum CHECKERS_GAME_UI_STATE {
		CHECKERS_GAME_UI_STATE_NOT_INITIALISED,
		CHECKERS_GAME_UI_STATE_INITIALISED,
		CHECKERS_GAME_UI_STATE_SELECTED,
		CHECKERS_GAME_UI_STATE_GAMEOVER
	};

	class CheckersGameObject: public enable_shared_from_this<CheckersGameObject> {
	public:
		shared_ptr<IModel> graphicsModel;
		shared_ptr<ICheckersTokenModel> gameModel;

		static const float TOKEN_Y_POSITION;

		CheckersGame& parent;

		CheckersGameObject(CheckersGame& parent, const shared_ptr<IModel>& graphicsModel, const shared_ptr<ICheckersTokenModel>& gameModel);
		void update();
	};
	
	static const unsigned int FRAMES_PER_SECOND;

	static const vec3 EYE_LOCATION;

	static const float TOKEN_THICKNESS;
	static const float KING_TOKEN_THICKNESS;

	static const float SQUARE_WIDTH;
	static const unsigned int NUMBER_OF_SQUARES;

	static const vec3 FIRST_CELL_SPACE_POSITION;

	static const vec3 UP;
	static const vec3 BOARD_POSITION;

	static const unsigned int MILLISECONDS_PER_FRAME;

	static vec3 calculateFirstCellPosition();

	void initialiseSDL();
	
	unique_ptr<Graphics> graphicsManager;

	set<shared_ptr<CheckersGameObject>> gameObjects;

	shared_ptr<CheckersModel> checkersModel;
	shared_ptr<ICheckersAi> checkersAi;

	shared_ptr<Model<TexturedMaterial>> boardModel;
	shared_ptr<Model<SolidColoredMaterial>> enemyTokenModel;
	shared_ptr<Model<SolidColoredMaterial>> playerTokenModel;
	
	shared_ptr<Model<SolidColoredMaterial>> positionalMarkerModel;

	weak_ptr<Mesh> tokenMesh;
	weak_ptr<Mesh> kingedTokenMesh;

	vector<shared_ptr<Model<SolidColoredMaterial>>> positionalMarkers;
	shared_ptr<Model<TexturedMaterial>> scoreBoardModel;

	shared_ptr<Model<SolidColoredMaterial>> turnMarker;
	SolidColoredMaterial enemyTokenMaterial;
	SolidColoredMaterial playerTokenMaterial;

	bool quit = false;

	void initialise();
	void initialiseMaterials();

	void initialiseModels();
	void initialiseGameObjects();
	
	bool tryGetBoardSpaceFromScreenSpace(const vec3& screenSpace, vec2* boardSpacePosition);
	vec3 getScreenSpaceFromBoardSpace(const vec2& boardSpacePosition);

	weak_ptr<Mesh> createTokenMesh(unsigned int numberOfLayers);

	void tokenPushVertex(vector<GLfloat>& vertices, vec2 coordinateInCircle, float height);
	void tokenPushVertex(vector<GLfloat>& vertices, float height);
	void tokenPushNormal(vector<GLfloat>& normals, float x, float y, float z);
	void tokenPushNormal(vector<GLfloat>& normals, vec2 normal);

	CHECKERS_GAME_UI_STATE state;

	shared_ptr<ICheckersTokenModel> selectedToken;
	vector<shared_ptr<ILegalMove>> currentLegalMoves;
	void transitionToGameOverState();
	void transitionToSelectedToken(const shared_ptr<ICheckersTokenModel>& selectedToken);
	void transitionToInitialisedFromSelected();

	void clearPositionalMarkers();
	void placePositionalMarkers();

	void processEvents();
	void update();

public:
	CheckersGame();

	void run();

	~CheckersGame();
};

#endif // checkers_game_h__
