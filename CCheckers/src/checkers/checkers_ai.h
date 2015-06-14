#ifndef __checkers_ai__
#define __checkers_ai__

#include <random>
#include <algorithm>
#include <functional>
#include <checkers/checkers_model.h>

using namespace std;
using namespace std::placeholders;

class ICheckersAi {
public:
	virtual shared_ptr<ILegalMove> getBestMove(const shared_ptr<const CheckersModel>& checkersModel) = 0;
};


class RandomCheckersAi : public ICheckersAi {
private:
	random_device randomDevice;
	unique_ptr<mt19937> generator;

public:
	RandomCheckersAi();
	shared_ptr<ILegalMove> getBestMove(const shared_ptr<const CheckersModel>& checkersModel);

};

class FitnessBasedCheckersAI : public ICheckersAi {
public:
	shared_ptr<ILegalMove> getBestMove(const shared_ptr<const CheckersModel>& checkersModel);

};
#endif