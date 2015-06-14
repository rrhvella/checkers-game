#include "checkers_game.h"

#include <SDL.h>

const unsigned int CheckersGame::FRAMES_PER_SECOND = 60;

const float CheckersGame::TOKEN_THICKNESS = 0.25f;
const float CheckersGame::KING_TOKEN_THICKNESS = 0.5f;

const float CheckersGame::SQUARE_WIDTH = 1.0f;
const unsigned int CheckersGame::NUMBER_OF_SQUARES = 8;

const vec3 CheckersGame::UP(0.0f, 1.0f, 0.0f);
const vec3 CheckersGame::BOARD_POSITION(0.0f, 0.0f, 0.0f);

const vec3 CheckersGame::FIRST_CELL_SPACE_POSITION = CheckersGame::calculateFirstCellPosition();

const vec3 CheckersGame::EYE_LOCATION(0.0f, 7.0f, 9.0f);

const unsigned int CheckersGame::MILLISECONDS_PER_FRAME = (unsigned int)(1000.0 / FRAMES_PER_SECOND);

void CheckersGame::CheckersGameObject::update() {
	const vec2& position = gameModel->getPosition();

	if (gameModel->getIsKing()) {
		graphicsModel->setMesh(parent.kingedTokenMesh);
	} else {
		graphicsModel->setMesh(parent.tokenMesh);
	}

	float tokenHeight = ((gameModel->getIsKing()) ? KING_TOKEN_THICKNESS : TOKEN_THICKNESS);
	graphicsModel->setTranslation(parent.getScreenSpaceFromBoardSpace(position) + vec3(0, tokenHeight * 0.5f, 0));
	graphicsModel->setScale(vec3(SQUARE_WIDTH, tokenHeight, SQUARE_WIDTH));
}

CheckersGame::CheckersGameObject::CheckersGameObject(CheckersGame& _parent, const shared_ptr<IModel>& graphicsModel, const shared_ptr<ICheckersTokenModel>& gameModel) : parent(_parent)  {
	this->graphicsModel = graphicsModel;
	this->gameModel = gameModel;
}

vec3 CheckersGame::calculateFirstCellPosition() {
	float xzOffset = CheckersGame::SQUARE_WIDTH * CheckersGame::NUMBER_OF_SQUARES * 0.5f - 0.5f;
	return CheckersGame::BOARD_POSITION - vec3(xzOffset, 0, xzOffset);
}

void CheckersGame::processEvents() {
	SDL_Event e;

	while (SDL_PollEvent(&e)) {

		if (e.type == SDL_QUIT) {
			quit = true;
		}
		else if ((state != CHECKERS_GAME_UI_STATE_INITIALISED && state != CHECKERS_GAME_UI_STATE_SELECTED) || checkersModel->getState() != CheckersModel::CHECKERS_MODEL_STATE_PLAYER_TURN) {
			return;
		}
		else if (e.type == SDL_MOUSEBUTTONDOWN) {
			int x;
			int y;

			Uint8 mouse_state = SDL_GetMouseState(&x, &y);

			if (mouse_state & SDL_BUTTON_LMASK) {
				vec2 mouseCoordinates((float)x, (float)y);
				Ray& mouseRay = graphicsManager->screenCoordinateToRay(mouseCoordinates);
				vec3 collissionWithBoardPlane;

				if (mouseRay.tryIntersectWithPlane(UP, BOARD_POSITION, &collissionWithBoardPlane)) {
					vec2 boardSpacePosition;

					if (tryGetBoardSpaceFromScreenSpace(collissionWithBoardPlane, &boardSpacePosition)) {
						if (this->state == CHECKERS_GAME_UI_STATE_SELECTED) {
							for (vector<shared_ptr<ILegalMove>>::iterator it = currentLegalMoves.begin(); it != currentLegalMoves.end(); it++) {
								if ((*it)->getPosition() == boardSpacePosition) {
									checkersModel->applyLegalMove(*it);
									break;
								}
							}

							transitionToInitialisedFromSelected();
						}
						else {
							shared_ptr<ICheckersTokenModel> selectedToken = checkersModel->getTokenAt(boardSpacePosition);

							if (selectedToken) {
								transitionToSelectedToken(selectedToken);
							}
						}
					}
				}
			}
			else {
				transitionToInitialisedFromSelected();
			}
		}
	}
}

void CheckersGame::transitionToGameOverState() {
	weak_ptr<Texture> scoreBoardTexture;
	switch (checkersModel->getWinner()) {
	case CheckersModel::CHECKERS_MODEL_WINNER_PLAYER:
		scoreBoardTexture = graphicsManager->loadTexture("resources/PlayerWon.png");
			break;

	case CheckersModel::CHECKERS_MODEL_WINNER_ENEMY:
		scoreBoardTexture = graphicsManager->loadTexture("resources/PlayerLost.png");
			break;

	case CheckersModel::CHECKERS_MODEL_NO_WINNER:
	case CheckersModel::CHECKERS_MODEL_GAME_UNFINISHED:
		scoreBoardTexture = graphicsManager->loadTexture("resources/StaleMate.png");
		break;
	}

	TexturedMaterial material;

	material.texture = scoreBoardTexture;

	scoreBoardModel = graphicsManager->createModel<TexturedMaterial>(graphicsManager->getPlaneMesh(), material, graphicsManager->getTexturedProgram());
	scoreBoardModel->setScale(vec3(4, 1, 2));
	scoreBoardModel->setTranslation(vec3(0, 3, 0));
	scoreBoardModel->setRotation(atan2f(EYE_LOCATION.z, EYE_LOCATION.y), vec3(1, 0, 0));

	state = CHECKERS_GAME_UI_STATE_GAMEOVER;
}

void CheckersGame::update() {


	for (set<shared_ptr<CheckersGameObject>>::iterator it = gameObjects.begin(); it != gameObjects.end();) {
		const shared_ptr<CheckersGameObject>& gameObject = *it;
		gameObject->update();

		if (gameObject->gameModel->getIsRemoved()) {
			graphicsManager->removeModel(gameObject->graphicsModel);
			it = gameObjects.erase(it);
		}
		else {
			it++;
		}
	}

	if (state == CHECKERS_GAME_UI_STATE_GAMEOVER) {
		return;
	} 
	else if (checkersModel->getState() == CheckersModel::CHECKERS_MODEL_STATE_GAME_OVER) {
		transitionToGameOverState();
		return;
	}

	if (checkersModel->getState() == CheckersModel::CHECKERS_MODEL_STATE_ENEMY_TURN) {
		turnMarker->setMaterial(enemyTokenMaterial);
		checkersModel->applyLegalMove(checkersAi->getBestMove(checkersModel));
	}
	else {
		turnMarker->setMaterial(playerTokenMaterial);
	}
}

void CheckersGame::run() {
	initialise();

	while (!quit) {
		unsigned int ticks = SDL_GetTicks();

		processEvents();
		update();
		graphicsManager->draw();
		
		unsigned int difference = (SDL_GetTicks() - ticks);

		if (difference <= MILLISECONDS_PER_FRAME) {
			SDL_Delay(MILLISECONDS_PER_FRAME - difference);
		}
	}
}

CheckersGame::CheckersGame() {
	state = CHECKERS_GAME_UI_STATE_NOT_INITIALISED;
}

void CheckersGame::initialiseSDL() {
	if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0) {
		throw exception(SDL_GetError());
	}
}

void CheckersGame::initialise() {
	initialiseSDL();

	graphicsManager = make_unique<Graphics>("Bob's Checkers", 800, 600, vec4(0.0f, 0.7f, 0.7f, 1.f));
	checkersModel = make_shared<CheckersModel>();
	checkersAi = make_unique<FitnessBasedCheckersAI>();
	
	graphicsManager->setView(EYE_LOCATION, vec3(0, 0, 0), UP);
	graphicsManager->setProjection(45.0f, 0.1f, 100.0f);

	graphicsManager->setAmbientLight(LightInfo(vec3(1.0f, 1.0f, 1.0f), 0.6f));
	graphicsManager->setDiffuseLight(DirectionalLightInfo(vec3(1.0f, 1.0f, 1.0f), vec3(0.77f, -0.77f, 0.77f), 0.3f));
	
	initialiseModels();
	initialiseGameObjects();

	state = CHECKERS_GAME_UI_STATE_INITIALISED;
}

void CheckersGame::transitionToInitialisedFromSelected() {
	clearPositionalMarkers();
	state = CHECKERS_GAME_UI_STATE_INITIALISED;
}

void CheckersGame::transitionToSelectedToken(const shared_ptr<ICheckersTokenModel>& selectedToken) {
	this->selectedToken = selectedToken;
	this->currentLegalMoves = selectedToken->getLegalMoves();

	clearPositionalMarkers();
	placePositionalMarkers();

	state = CHECKERS_GAME_UI_STATE_SELECTED;
}

vec3 CheckersGame::getScreenSpaceFromBoardSpace(const vec2& boardSpacePosition) {
	vec2& xyPosition = vec2(FIRST_CELL_SPACE_POSITION.x, FIRST_CELL_SPACE_POSITION.z) + round(vec2(boardSpacePosition.x * SQUARE_WIDTH, boardSpacePosition.y * SQUARE_WIDTH));
	return vec3(xyPosition.x, 0, xyPosition.y);
}

bool CheckersGame::tryGetBoardSpaceFromScreenSpace(const vec3& screenSpace, vec2* boardSpacePosition) {
	vec2& result = round(vec2(screenSpace.x / SQUARE_WIDTH, screenSpace.z / SQUARE_WIDTH) - vec2(FIRST_CELL_SPACE_POSITION.x, FIRST_CELL_SPACE_POSITION.z));

	if (result.x < 0 || result.y < 0 || result.x >= NUMBER_OF_SQUARES || result.y >= NUMBER_OF_SQUARES) {
		return false;
	}

	*boardSpacePosition = result;
	return true;
}

void CheckersGame::clearPositionalMarkers() {
	for (vector<shared_ptr<Model<SolidColoredMaterial>>>::iterator it = positionalMarkers.begin(); it < positionalMarkers.end(); it++) {
		graphicsManager->removeModel(*it);
	}

	positionalMarkers.clear();
}

void CheckersGame::placePositionalMarkers() {
	for (vector<shared_ptr<ILegalMove>>::iterator it = currentLegalMoves.begin(); it < currentLegalMoves.end(); it++) {
		const shared_ptr<Model<SolidColoredMaterial>>& model = graphicsManager->createModel<SolidColoredMaterial>(positionalMarkerModel);
		
		vec2& boardSpacePosition = (*it)->getPosition();
		vec3& modelSpacePosition = FIRST_CELL_SPACE_POSITION + vec3(boardSpacePosition.x, 0.005f, boardSpacePosition.y);

		model->setTranslation(modelSpacePosition);

		positionalMarkers.push_back(model);
	}
}

CheckersGame::~CheckersGame() {
	graphicsManager.reset();
	SDL_Quit();
}

weak_ptr<Mesh> CheckersGame::createTokenMesh(unsigned int numberOfLayers) {
	const int NUMBER_OF_SEGMENTS = 50;
	const int VERTICES_PER_SEGMENT = 12;
	const int VERTEX_SIZE = 3;
	const int VERTICES_PER_TRIANGLE = 3;
	const int TEXTURE_COORDINATE_SIZE = 2;
	const float DISTANCE_BETWEEN_TOKENS = 0.4f;
	const int TOTAL_NUMBER_OF_VERTICES = numberOfLayers * NUMBER_OF_SEGMENTS * VERTICES_PER_SEGMENT;

	vector<GLuint> indices;
	vector<GLfloat> normals;
	vector<GLfloat> vertices;

	float angleDiff = (2 * pi<float>()) / NUMBER_OF_SEGMENTS;

	float height = (1.0f - DISTANCE_BETWEEN_TOKENS * (numberOfLayers - 1)) / numberOfLayers;
	float halfHeight = height * 0.5f;

	float start = -0.5f + halfHeight;

	int lastIndex = 0;

	for (int i = 0; i < numberOfLayers; i++) {
		float yPosition = start + i*(height + DISTANCE_BETWEEN_TOKENS);

		float top = yPosition + halfHeight;
		float bottom = yPosition - halfHeight;

		vec2 previousSegmentDirection;
		vec2 currentSegmentDirection(0, 1);

		for (int i = 0; i < NUMBER_OF_SEGMENTS; i++) {
			previousSegmentDirection = currentSegmentDirection;

			currentSegmentDirection = rotate(previousSegmentDirection, angleDiff);

			vec2 segmentStart = previousSegmentDirection*0.5f;
			vec2 segmentEnd = currentSegmentDirection*0.5f;

			//Top triangle
			tokenPushVertex(vertices, segmentStart, top);
			tokenPushVertex(vertices, top);
			tokenPushVertex(vertices, segmentEnd, top);

			tokenPushNormal(normals, 0, 1.0f, 0);
			tokenPushNormal(normals, 0, 1.0f, 0);
			tokenPushNormal(normals, 0, 1.0f, 0);

			//Left triangle
			tokenPushVertex(vertices, segmentStart, bottom);
			tokenPushVertex(vertices, segmentStart, top);
			tokenPushVertex(vertices, segmentEnd, bottom);

			tokenPushNormal(normals, previousSegmentDirection);
			tokenPushNormal(normals, previousSegmentDirection);
			tokenPushNormal(normals, currentSegmentDirection);

			//Right triangle
			tokenPushVertex(vertices, segmentEnd, bottom);
			tokenPushVertex(vertices, segmentStart, top);
			tokenPushVertex(vertices, segmentEnd, top);

			tokenPushNormal(normals, currentSegmentDirection);
			tokenPushNormal(normals, previousSegmentDirection);
			tokenPushNormal(normals, currentSegmentDirection);

			//Bottom triangle
			tokenPushVertex(vertices, segmentEnd, bottom);
			tokenPushVertex(vertices, bottom);
			tokenPushVertex(vertices, segmentStart, bottom);

			tokenPushNormal(normals, 0, -1.0f, 0);
			tokenPushNormal(normals, 0, -1.0f, 0);
			tokenPushNormal(normals, 0, -1.0f, 0);

			for (int j = 0; j < VERTICES_PER_SEGMENT; j++) {
				indices.push_back(lastIndex++);
			}
		}

	}

	return graphicsManager->createMesh(vertices.data(), normals.data(), VERTEX_SIZE, TEXTURE_COORDINATE_SIZE, NULL, indices.data(), TOTAL_NUMBER_OF_VERTICES, TOTAL_NUMBER_OF_VERTICES);
}

void CheckersGame::tokenPushVertex(vector<GLfloat>& vertices, vec2 coordinateInCircle, float yPosition) {
	vertices.push_back(coordinateInCircle.x);
	vertices.push_back(yPosition);
	vertices.push_back(coordinateInCircle.y);
}

void CheckersGame::tokenPushVertex(vector<GLfloat>& vertices, float yPosition) {
	vertices.push_back(0);
	vertices.push_back(yPosition);
	vertices.push_back(0);
}

void CheckersGame::tokenPushNormal(vector<GLfloat>& normals, vec2 normal) {
	tokenPushNormal(normals, normal.x, 0.0f, normal.y);
}

void CheckersGame::tokenPushNormal(vector<GLfloat>& normals, float x, float y, float z) {
	normals.push_back(x);
	normals.push_back(y);
	normals.push_back(z);
}

void CheckersGame::initialiseModels() {
	weak_ptr<Texture>& boardTexture = graphicsManager->loadTexture("resources/Board.png");
	TexturedMaterial boardMaterial(boardTexture);

	boardModel = graphicsManager->createModel<TexturedMaterial>(graphicsManager->getPlaneMesh(), boardMaterial, graphicsManager->getTexturedProgram());
	boardModel->setScale(vec3(SQUARE_WIDTH * NUMBER_OF_SQUARES, 1.0f, SQUARE_WIDTH * NUMBER_OF_SQUARES));
	boardModel->setTranslation(BOARD_POSITION);

	tokenMesh = createTokenMesh(1);
	kingedTokenMesh = createTokenMesh(2);

	enemyTokenMaterial = SolidColoredMaterial(vec4(0.0f, 0.0f, 1.0f, 1.0f));
	enemyTokenModel = make_shared<Model<SolidColoredMaterial>>(tokenMesh, enemyTokenMaterial, graphicsManager->getSolidColoredProgram());

	playerTokenMaterial = SolidColoredMaterial(vec4(1.0f, 0.0f, 0.0f, 1.0f));
	playerTokenModel = make_shared<Model<SolidColoredMaterial>>(tokenMesh, playerTokenMaterial, graphicsManager->getSolidColoredProgram());

	turnMarker = graphicsManager->createModel<SolidColoredMaterial>(kingedTokenMesh, playerTokenMaterial, graphicsManager->getSolidColoredProgram());
	turnMarker->setTranslation(BOARD_POSITION - vec3(SQUARE_WIDTH * NUMBER_OF_SQUARES * 0.5 + SQUARE_WIDTH * 3, 0, 0));
	turnMarker->setScale(vec3(SQUARE_WIDTH, KING_TOKEN_THICKNESS, SQUARE_WIDTH));

	SolidColoredMaterial positionMarkerMaterial(vec4(0.0f, 1.0f, 0.0f, 1.0f));
	positionalMarkerModel = make_shared<Model<SolidColoredMaterial>>(graphicsManager->getPlaneMesh(), positionMarkerMaterial, graphicsManager->getSolidColoredProgram());
	positionalMarkerModel->setScale(vec3(SQUARE_WIDTH, 1.0f, SQUARE_WIDTH));
}

void CheckersGame::initialiseGameObjects() {
	//TODO: Ideally if we are going to create a querying mechanism, it should be in this class.
	const vector<shared_ptr<ICheckersTokenModel>>& playerTokens = checkersModel->getPlayerTokens();
	const vector<shared_ptr<ICheckersTokenModel>>& enemyTokens = checkersModel->getEnemyTokens();

	for (vector<shared_ptr<ICheckersTokenModel>>::const_iterator it = playerTokens.begin(); it < playerTokens.end(); it++) {
		const shared_ptr<IModel>& model = graphicsManager->createModel<SolidColoredMaterial>(playerTokenModel);
		shared_ptr<CheckersGameObject>& playerObject = make_shared<CheckersGameObject>(*this, model, *it);
		
		gameObjects.insert(playerObject);
	}

	for (vector<shared_ptr<ICheckersTokenModel>>::const_iterator it = enemyTokens.begin(); it < enemyTokens.end(); it++) {
		const shared_ptr<IModel>& model = graphicsManager->createModel<SolidColoredMaterial>(enemyTokenModel);
		shared_ptr<CheckersGameObject>& enemyObject = make_shared<CheckersGameObject>(*this, model, *it);

		gameObjects.insert(enemyObject);
	}
}
