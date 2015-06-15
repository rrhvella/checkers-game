#include <checkers/checkers_ai.h>

RandomCheckersAi::RandomCheckersAi() {
	generator = make_unique<mt19937>(randomDevice());
}

shared_ptr<ILegalMove> RandomCheckersAi::getBestMove(const shared_ptr<const CheckersModel>& checkersModel) {
	vector<shared_ptr<ILegalMove>>& legalMoves = checkersModel->getLegalMoves();

	std::uniform_int_distribution<int> integerDistribution(0, legalMoves.size() - 1);

	return legalMoves[integerDistribution(*generator)];
}

bool sortByFitness(const pair<shared_ptr<ILegalMove>, float>& first, const pair<shared_ptr<ILegalMove>, float>& second) {
	return first.second > second.second;
}

float getFitnessForSide(CheckersModel& simulation, bool isEnemyTurn) {
	float KING_EXISTS_SCORE = 5.f;
	float TOKEN_EXISTS_SCORE = 1.f;
	float NEIGHBORED_BY_FRIEND_SCORE = 0.25f;
	float NEXT_TO_NON_THREATENING_ENEMY_SCORE = 20.f;
	float THREATENING_ENEMY_SCORE = 10.f;

	auto tokens = simulation.getTokensByIsEnemy(isEnemyTurn);
	float fitness = 0;

	for (auto it = tokens.begin(); it != tokens.end(); it++) {
		auto token = *it;
		
		fitness += (token->getIsKing()) ? KING_EXISTS_SCORE : TOKEN_EXISTS_SCORE;

		auto surroundingTokens = (*it)->getSurroundingTokens();

		for (auto surroundingTokenIt = surroundingTokens.begin(); surroundingTokenIt != surroundingTokens.end(); surroundingTokenIt++) {
			auto surroundingToken = *surroundingTokenIt;
			if (surroundingToken->getIsEnemy() == isEnemyTurn) {
				fitness += NEIGHBORED_BY_FRIEND_SCORE;
			}

			if (surroundingToken->getIsEnemy() == !isEnemyTurn) {
				auto surroundingTokenThreatenedTokens = surroundingToken->getThreatenedTokens();
				
				if (find(surroundingTokenThreatenedTokens.begin(), surroundingTokenThreatenedTokens.end(), token) == surroundingTokenThreatenedTokens.end()) {
					fitness += NEXT_TO_NON_THREATENING_ENEMY_SCORE;
				}
			}
		}

		fitness += token->getThreatenedTokens().size() * THREATENING_ENEMY_SCORE;
	}

	return fitness;
}

float getFitness(CheckersModel& simulation, shared_ptr<ILegalMove>& legalMove)
{
	bool isEnemyTurn = simulation.getIsEnemysTurn();

	simulation.applyLegalMove(legalMove);

	float fitness = getFitnessForSide(simulation, isEnemyTurn) - getFitnessForSide(simulation, !isEnemyTurn);

	simulation.popMove();

	return fitness;
}

shared_ptr<ILegalMove> FitnessBasedCheckersAI::getBestMove(const shared_ptr<const CheckersModel>& checkersModel) 
{
	auto originalLegalMoves = checkersModel->getLegalMoves();

	CheckersModel simulation(*checkersModel);

	auto legalMoves = simulation.getLegalMoves();
	vector<pair<shared_ptr<ILegalMove>, float>> legalMoveFitnessPairs;

	for (auto it = legalMoves.begin(); it != legalMoves.end(); it++) {
		pair<shared_ptr<ILegalMove>, float> currentPair;

		currentPair.first = *it;
		currentPair.second = getFitness(simulation, *it);

		legalMoveFitnessPairs.push_back(currentPair);
	}

	sort(legalMoveFitnessPairs.begin(), legalMoveFitnessPairs.end(), sortByFitness);
	auto selectedlegalMove = legalMoveFitnessPairs[0].first;

	for (auto it = originalLegalMoves.begin(); it != originalLegalMoves.end(); it++) {
		if ((*it)->getPosition() == selectedlegalMove->getPosition()) {
			return *it;
		}
	}

	return originalLegalMoves[0];
}