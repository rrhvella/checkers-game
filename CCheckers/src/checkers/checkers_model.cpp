#include "checkers_model.h"


bool CheckersModel::CheckersTokenModel::getIsKing() const {
	return isKing;
}

CheckersModel::CheckersTokenModel::CheckersTokenModel(CheckersTokenModel& other, CheckersModel& _parent) : parent(_parent) {
	this->isEnemy = other.isEnemy;
	this->isKing = other.isKing;
	this->position = other.position;
	this->isRemoved = other.isRemoved;
}


CheckersModel::CheckersTokenModel::CheckersTokenModel(bool isEnemy, const vec2& position, CheckersModel& _parent) : parent(_parent) {
	this->isEnemy = isEnemy;
	this->isKing = false;
	this->position = position;
	this->isRemoved = false;
}

vec2 CheckersModel::CheckersTokenModel::getPosition() const {
	return position;
}

bool CheckersModel::CheckersTokenModel::getIsRemoved() const {
	return isRemoved;
}

bool CheckersModel::CheckersTokenModel::getIsEnemy() const {
	return isEnemy;
}

vector<shared_ptr<ILegalMove>> CheckersModel::CheckersTokenModel::getLegalMoves() const {
	return *parent.legalMovesByBoardIndex[parent.getIndexFromCoordinates(position)];
}

vector<shared_ptr<ICheckersTokenModel>> CheckersModel::CheckersTokenModel::getSurroundingTokens() const {
	vector<vec2> relativePositionsToCheck;
	vector<shared_ptr<ICheckersTokenModel>> results;

	relativePositionsToCheck.push_back(vec2(1, 1));
	relativePositionsToCheck.push_back(vec2(-1, 1));
	relativePositionsToCheck.push_back(vec2(1, -1));
	relativePositionsToCheck.push_back(vec2(-1, -1));

	for (vector<vec2>::iterator it = relativePositionsToCheck.begin(); it != relativePositionsToCheck.end(); it++) {
		vec2& positionToCheck = position + *it;

		if (!parent.isCoordinateInBounds(positionToCheck)) {
			continue;
		}

		auto token = parent.getTokenAt(positionToCheck);
		if (token) {
			results.push_back(token);
		}
	}

	return results;
}

vector<shared_ptr<ICheckersTokenModel>> CheckersModel::CheckersTokenModel::getThreatenedTokens() const {
	vector<vec2> relativePositionsToCheck;
	vector<shared_ptr<ICheckersTokenModel>> results;

	int movementDirection = parent.getMovementDirectionFromTokenType(isEnemy);

	relativePositionsToCheck.push_back(vec2(1, movementDirection));
	relativePositionsToCheck.push_back(vec2(-1, movementDirection));

	if (isKing) {
		relativePositionsToCheck.push_back(vec2(1, -movementDirection));
		relativePositionsToCheck.push_back(vec2(-1, -movementDirection));
	}

	for (vector<vec2>::iterator it = relativePositionsToCheck.begin(); it != relativePositionsToCheck.end(); it++) {
		vec2& jumpPosition = position + *it * (float)JUMP_DISTANCE;
		shared_ptr<CheckersModel::CheckersTokenModel> jumpedToken;

		if (isLegalJumpMove(jumpPosition, &jumpedToken)) {
			results.push_back(jumpedToken);
		}
	}

	return results;
}

vector<shared_ptr<CheckersModel::LegalMove>> CheckersModel::CheckersTokenModel::calculateLegalMoves(bool skipNormalMoves) {
	vector<shared_ptr<LegalMove>> results;

	if (isEnemy != parent.isEnemysTurn) {
		return results;
	}

	vector<vec2> relativePositionsToCheck;

	int movementDirection = parent.getMovementDirectionFromTokenType(isEnemy);

	relativePositionsToCheck.push_back(vec2(1, movementDirection));
	relativePositionsToCheck.push_back(vec2(-1, movementDirection));

	if (isKing) {
		relativePositionsToCheck.push_back(vec2(1, -movementDirection));
		relativePositionsToCheck.push_back(vec2(-1, -movementDirection));
	}

	for (vector<vec2>::iterator it = relativePositionsToCheck.begin(); it != relativePositionsToCheck.end(); it++) {
		vec2& positionToCheck = position + *it;

		if (!skipNormalMoves && isLegalMove(positionToCheck)) {
			results.push_back(make_shared<LegalMove>(parent.turn, shared_from_this(), shared_ptr<CheckersTokenModel>(), positionToCheck, parent.isEnemysTurn));
		}

		shared_ptr<CheckersModel::CheckersTokenModel> jumpedToken;
		vec2& jumpPosition = position + *it * (float)JUMP_DISTANCE;

		if (isLegalJumpMove(jumpPosition, &jumpedToken)) {
			if (!skipNormalMoves) {
				skipNormalMoves = true;
				results.clear();
			}

			results.push_back(make_shared<LegalMove>(parent.turn, shared_from_this(), jumpedToken, jumpPosition, parent.isEnemysTurn));
		}
	}

	return results;
}

bool CheckersModel::CheckersTokenModel::canBeCrowned() const {
	if (isKing) {
		return false;
	}

	int targetY = parent.getStartingYFromTokenType(!isEnemy);

	return targetY == position.y;
}

bool CheckersModel::CheckersTokenModel::isLegalMove(const vec2& positionToCheck) const {
	if (!parent.isCoordinateInBounds(positionToCheck)) {
		return false;
	}

	int index = parent.getIndexFromCoordinates(positionToCheck);

	if (parent.boardMap[index]) {
		return false;
	}

	return true;
}

bool CheckersModel::CheckersTokenModel::isLegalJumpMove(const vec2& positionToCheck, shared_ptr<CheckersModel::CheckersTokenModel>* jumpedToken) const {
	if (!isLegalMove(positionToCheck)) {
		return false;
	}
	
	auto direction = (positionToCheck - this->position);

	int movementDirection = parent.getMovementDirectionFromTokenType(isEnemy);
	int manhattanDistance = abs(direction.x);

	if (manhattanDistance != JUMP_DISTANCE) {
		return false;
	}

	auto jumpedTokenOffset = direction / (float)manhattanDistance;
	int jumpedIndex = parent.getIndexFromCoordinates(position + jumpedTokenOffset);

	if (!parent.boardMap[jumpedIndex] || parent.boardMap[jumpedIndex]->isEnemy == isEnemy) {
		return false;
	}

	if (jumpedToken != NULL) {
		*jumpedToken = parent.boardMap[jumpedIndex];
	}

	return true;
}

CheckersModel::LegalMove::LegalMove(unsigned int turn, const shared_ptr<CheckersTokenModel>& targetToken, const shared_ptr<CheckersTokenModel>& jumpedToken, const vec2& position, bool wasEnemysTurn) {
	this->turn = turn;
	
	this->targetToken = targetToken;
	this->jumpedToken = jumpedToken;

	this->oldPosition = targetToken->getPosition();
	this->position = position;
	
	this->wasKing = targetToken->getIsKing();

	this->wasEnemysTurn = wasEnemysTurn;
}

vec2 CheckersModel::LegalMove::getPosition() {
	return this->position;
}

CheckersModel::CheckersModel() {
	placeTokens(true);
	placeTokens(false);

	isEnemysTurn = false;
	turn = 0;

	winner = CHECKERS_MODEL_GAME_UNFINISHED;

	analyzeMovesAndBeginTurn(false);
}

CheckersModel::CheckersModel(const CheckersModel& other) {
	for (int i = 0; i < BOARD_TOTAL_NUMBER_OF_SQUARES; i++) {
		if (other.boardMap[i]) {
			boardMap[i] = make_shared<CheckersTokenModel>(*other.boardMap[i], *this);
		}
	}

	isEnemysTurn = other.isEnemysTurn;
	turn = 0;

	winner = other.winner;

	analyzeMovesAndBeginTurn(false);
}

int CheckersModel::getMovementDirectionFromTokenType(bool isEnemy) {
	return (isEnemy) ? 1 : -1;
}

int CheckersModel::getStartingYFromTokenType(bool isEnemy) {
	return (isEnemy) ? 0 : BOARD_SIDE_NUMBER_OF_SQUARES - 1;
}

void CheckersModel::placeTokens(bool isEnemy) {
	int n = BOARD_SIDE_NUMBER_OF_SQUARES / 2;

	int movementDirection = getMovementDirectionFromTokenType(isEnemy);
	int start = getStartingYFromTokenType(isEnemy);

	for (int i = 0; i < n - 1; i++) {
		for (int j = 0; j < n; j++) {
			int x = ((j * 2) + ((i + 1) % 2)) * movementDirection + start; 
			int y = i * movementDirection + start;

			boardMap[getIndexFromCoordinates(x, y)] = make_shared<CheckersTokenModel>(isEnemy, vec2(x, y), *this);
		}
	}
}

CheckersModel::CheckersModelState CheckersModel::getState() const {
	if (winner != CHECKERS_MODEL_GAME_UNFINISHED) {
		return CHECKERS_MODEL_STATE_GAME_OVER;
	} 
	else if (isEnemysTurn) {
		return CHECKERS_MODEL_STATE_ENEMY_TURN;
	}
	else {
		return CHECKERS_MODEL_STATE_PLAYER_TURN;
	}
}

CheckersModel::CheckersModelWinner CheckersModel::getWinner() const {
	return winner;
}


vector<shared_ptr<ILegalMove>> CheckersModel::getLegalMoves() const {
	return currentLegalMoves;
}

vector<shared_ptr<CheckersModel::LegalMove>> CheckersModel::calculateLegalMoves() {
	vector<shared_ptr<LegalMove>> result;
	bool hasJumpMoves = false; 


	for (int i = 0; i < BOARD_TOTAL_NUMBER_OF_SQUARES; i++) {
		if (boardMap[i] && (boardMap[i]->isEnemy == isEnemysTurn)) {
			vector<shared_ptr<LegalMove>> tokenLegalMoves = boardMap[i]->calculateLegalMoves(hasJumpMoves);

			for (vector<shared_ptr<LegalMove>>::iterator it = tokenLegalMoves.begin(); it != tokenLegalMoves.end(); it++) {
				if (!hasJumpMoves && (*it)->jumpedToken) {
					hasJumpMoves = true;
					result.clear();
				}

				result.push_back(*it);
			}
		}
	}

	return result;
}

void CheckersModel::beginTurn(vector<shared_ptr<CheckersModel::LegalMove>> legalMoves) {
	currentLegalMoves.clear();

	for (int i = 0; i < BOARD_TOTAL_NUMBER_OF_SQUARES; i++) {
		if (boardMap[i]) {
			legalMovesByBoardIndex[i] = make_shared<vector<shared_ptr<ILegalMove>>>();
		} else {
			legalMovesByBoardIndex[i] = shared_ptr<vector<shared_ptr<ILegalMove>>>();
		}
	}

	for (vector<shared_ptr<LegalMove>>::const_iterator it = legalMoves.begin(); it != legalMoves.end(); it++) {
		currentLegalMoves.push_back(*it);

		int x = getIndexFromCoordinates((*it)->targetToken->position);
		legalMovesByBoardIndex[x]->push_back(*it);
	}
}

void CheckersModel::advanceToCurrentPlayersNextTurn(const shared_ptr<CheckersTokenModel>& tokenPerformingJump) {
	turn++;

	vector<shared_ptr<LegalMove>> legalMoves = tokenPerformingJump->calculateLegalMoves(true);
	if (legalMoves.size() > 0) {
		beginTurn(legalMoves);
	}
	else {
		advanceToNextPlayersTurn(false);
	}
}

void CheckersModel::analyzeMovesAndBeginTurn(bool previousTurnHadNoMoves) {
	vector<shared_ptr<LegalMove>> legalMoves = calculateLegalMoves();
	if (legalMoves.size() == 0) {
		if (!previousTurnHadNoMoves) {
			advanceToNextPlayersTurn(true);
		}
		else {
			winner = CHECKERS_MODEL_NO_WINNER;
		}
	} else {
		beginTurn(legalMoves);
	}
}

void CheckersModel::advanceToNextPlayersTurn(bool previousTurnHadNoMoves) {
	turn++;
	isEnemysTurn = !isEnemysTurn;

	analyzeMovesAndBeginTurn(previousTurnHadNoMoves);
}

void CheckersModel::popMove() {
	shared_ptr<LegalMove> lastLegalMove = history.top();
	history.pop();

	shared_ptr<CheckersTokenModel>& targetToken = lastLegalMove->targetToken;
	shared_ptr<CheckersTokenModel>& jumpedToken = lastLegalMove->jumpedToken;

	int oldIndex = getIndexFromCoordinates(lastLegalMove->oldPosition);
	int newIndex = getIndexFromCoordinates(lastLegalMove->position);

	boardMap[oldIndex] = lastLegalMove->targetToken;
	boardMap[newIndex] = shared_ptr<CheckersTokenModel>();

	targetToken->position = lastLegalMove->oldPosition;
	targetToken->isKing = lastLegalMove->wasKing;

	if (jumpedToken) {
		int jumpedTokenIndex = getIndexFromCoordinates(jumpedToken->position);
		boardMap[jumpedTokenIndex] = jumpedToken;
		jumpedToken->isRemoved = false;
	}

	turn = lastLegalMove->turn;
	isEnemysTurn = lastLegalMove->wasEnemysTurn;

	analyzeMovesAndBeginTurn(false);
}

void CheckersModel::applyLegalMove(shared_ptr<ILegalMove>& legalMove) {
	shared_ptr<LegalMove>& castLegalMove = dynamic_pointer_cast<LegalMove>(legalMove);

	if (castLegalMove->turn != turn) {
		throw exception("Attempted to perform expired move");
	}

	shared_ptr<CheckersTokenModel>& targetToken = castLegalMove->targetToken;

	int oldIndex = getIndexFromCoordinates(castLegalMove->oldPosition);
	int newIndex = getIndexFromCoordinates(castLegalMove->position);

	boardMap[oldIndex] = shared_ptr<CheckersTokenModel>();
	boardMap[newIndex] = castLegalMove->targetToken;

	targetToken->position = castLegalMove->position;

	if (targetToken->canBeCrowned()) {
		targetToken->isKing = true;
	}

	if (castLegalMove->jumpedToken) {
		removeToken(castLegalMove->jumpedToken);

		if (countTokensByIsEnemy(!isEnemysTurn) == 0) {
			if (isEnemysTurn) {
				winner = CHECKERS_MODEL_WINNER_ENEMY;
			}
			else {
				winner = CHECKERS_MODEL_WINNER_PLAYER;
			}
		}
		else {
			advanceToCurrentPlayersNextTurn(castLegalMove->targetToken);
		}
	}
	else {
		advanceToNextPlayersTurn(false);
	}

	history.push(castLegalMove);
}

void CheckersModel::removeToken(shared_ptr<CheckersTokenModel>& token) {
	int oldIndex = getIndexFromCoordinates(token->getPosition());

	boardMap[oldIndex] = shared_ptr<CheckersTokenModel>();

	token->isRemoved = true;
}

int CheckersModel::getIndexFromCoordinates(int x, int y) const {
	return y*BOARD_SIDE_NUMBER_OF_SQUARES + x;
}

int CheckersModel::getIndexFromCoordinates(const vec2& coordinates) const {
	return getIndexFromCoordinates(coordinates.x, coordinates.y);
}

int CheckersModel::countTokensByIsEnemy(bool isEnemy) const {
	int total = 0;

	for (int i = 0; i < BOARD_TOTAL_NUMBER_OF_SQUARES; i++) {
		if (boardMap[i] && (boardMap[i]->isEnemy == isEnemy)) {
			total += 1;
		}
	}

	return total;
}

bool CheckersModel::isCoordinateInBounds(const vec2& positionToCheck) const {
	return positionToCheck.x >= 0 && positionToCheck.y >= 0 && positionToCheck.x < BOARD_SIDE_NUMBER_OF_SQUARES && positionToCheck.y < BOARD_SIDE_NUMBER_OF_SQUARES;
}


vector<shared_ptr<ICheckersTokenModel>> CheckersModel::getTokensByIsEnemy(bool isEnemy) const {
	vector<shared_ptr<ICheckersTokenModel>> result;

	for (int i = 0; i < BOARD_TOTAL_NUMBER_OF_SQUARES; i++) {
		if (boardMap[i] && (boardMap[i]->isEnemy == isEnemy)) {
			result.push_back(boardMap[i]);
		}
	}

	return result;
}

shared_ptr<ICheckersTokenModel> CheckersModel::getTokenAt(const vec2& boardPosition) const {
	return boardMap[getIndexFromCoordinates(boardPosition)];
}

bool CheckersModel::getIsEnemysTurn() const {
	return isEnemysTurn;
}

vector<shared_ptr<ICheckersTokenModel>> CheckersModel::getEnemyTokens() const {
	return getTokensByIsEnemy(true);
}

vector<shared_ptr<ICheckersTokenModel>> CheckersModel::getPlayerTokens() const {
	return getTokensByIsEnemy(false);
}