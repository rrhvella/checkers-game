#ifndef checkers_model_h__
#define checkers_model_h__

#include <map>
#include <memory>
#include <stack>
#include <vector>

#include <glm/glm.hpp>

using namespace std;
using namespace glm;

struct ILegalMove {
	virtual vec2 getPosition() = 0;
};


struct ICheckersTokenModel {
public:
	virtual vec2 getPosition() const = 0;
	virtual vector<shared_ptr<ILegalMove>> getLegalMoves() const = 0;

	virtual vector<shared_ptr<ICheckersTokenModel>> getSurroundingTokens() const = 0;
	virtual vector<shared_ptr<ICheckersTokenModel>> getThreatenedTokens() const = 0;

	virtual bool getIsEnemy() const = 0;
	virtual bool getIsRemoved() const = 0;
	virtual bool getIsKing() const = 0;
};

class CheckersModel {
public:
	enum CheckersModelWinner {
		CHECKERS_MODEL_WINNER_PLAYER,
		CHECKERS_MODEL_WINNER_ENEMY,
		CHECKERS_MODEL_NO_WINNER,
		CHECKERS_MODEL_GAME_UNFINISHED
	};

	enum CheckersModelState {
		CHECKERS_MODEL_STATE_GAME_OVER,
		CHECKERS_MODEL_STATE_PLAYER_TURN,
		CHECKERS_MODEL_STATE_ENEMY_TURN
	};

private:
	static const int BOARD_SIDE_NUMBER_OF_SQUARES = 8;
	static const int JUMP_DISTANCE = 2;
	static const int BOARD_TOTAL_NUMBER_OF_SQUARES = BOARD_SIDE_NUMBER_OF_SQUARES * BOARD_SIDE_NUMBER_OF_SQUARES;

	struct LegalMove;

	struct CheckersTokenModel : public ICheckersTokenModel, public enable_shared_from_this<CheckersTokenModel> {
		public:
			bool isEnemy;
			bool isKing;
			bool isRemoved;
			vec2 position;

			CheckersModel& parent;

			CheckersTokenModel(CheckersTokenModel& other, CheckersModel& parent);
			CheckersTokenModel(bool isEnemy, const vec2& position, CheckersModel& parent);

			vector<shared_ptr<ICheckersTokenModel>> getSurroundingTokens() const;
			vector<shared_ptr<ICheckersTokenModel>> getThreatenedTokens() const;

			bool getIsEnemy() const;
			bool getIsKing() const;
			bool getIsRemoved() const;
			vec2 getPosition() const;

			bool canBeCrowned() const;

			bool isLegalMove(const vec2& newPosition) const;
			bool isLegalJumpMove(const vec2& newPosition, shared_ptr<CheckersTokenModel>* jumpedModel) const;
			
			vector<shared_ptr<ILegalMove>> getLegalMoves() const;
			vector<shared_ptr<LegalMove>> calculateLegalMoves(bool skipNormalMoves);
	};

	struct LegalMove:ILegalMove {
		unsigned int turn;
		shared_ptr<CheckersTokenModel> targetToken;
		shared_ptr<CheckersTokenModel> jumpedToken;
		
		vec2 position;
		vec2 oldPosition;

		bool wasEnemysTurn;
		bool wasKing;

		LegalMove(unsigned int turn, const shared_ptr<CheckersTokenModel>& targetToken, const shared_ptr<CheckersTokenModel>& jumpedToken, const vec2& position, bool wasEnemysTurn);

		vec2 getPosition();
	};

	shared_ptr<CheckersTokenModel> boardMap[BOARD_TOTAL_NUMBER_OF_SQUARES];
	shared_ptr<vector<shared_ptr<ILegalMove>>> legalMovesByBoardIndex[BOARD_TOTAL_NUMBER_OF_SQUARES];

	vector<shared_ptr<ILegalMove>> currentLegalMoves;

	bool isEnemysTurn;
	unsigned int turn;

	void nextTurn();
	void removeToken(shared_ptr<CheckersTokenModel>& token);

	void placeTokens(bool isEnemy);

	int getMovementDirectionFromTokenType(bool isEnemy); 
	int getStartingYFromTokenType(bool isEnemy);

	int getIndexFromCoordinates(int x, int y) const;
	int getIndexFromCoordinates(const vec2& coordinates) const;

	void beginTurn(vector<shared_ptr<LegalMove>> legalMoves);

	vector<shared_ptr<LegalMove>> calculateLegalMoves();
	stack<shared_ptr<LegalMove>> history;

	void analyzeMovesAndBeginTurn(bool previousTurnHadNoMoves);
	void advanceToCurrentPlayersNextTurn(const shared_ptr<CheckersTokenModel>& tokenPerformingJump);
	void advanceToNextPlayersTurn(bool previousTurnHadNoMoves);

	CheckersModelWinner winner;

public:
	CheckersModel();
	CheckersModel(const CheckersModel& other);

	CheckersModelWinner getWinner() const;
	CheckersModelState getState() const;

	vector<shared_ptr<ICheckersTokenModel>> getEnemyTokens() const;
	vector<shared_ptr<ICheckersTokenModel>> getPlayerTokens() const;

	vector<shared_ptr<ILegalMove>> getLegalMoves() const;

	shared_ptr<ICheckersTokenModel> getTokenAt(const vec2& boardPosition) const;

	bool getIsEnemysTurn() const;

	void popMove();
	void applyLegalMove(shared_ptr<ILegalMove>& legalMove);

	bool isCoordinateInBounds(const vec2& positionToCheck) const;

	vector<shared_ptr<ICheckersTokenModel>> getTokensByIsEnemy(bool isEnemy) const;
	int countTokensByIsEnemy(bool isEnemy) const;
};

#endif // checkers_model_h__