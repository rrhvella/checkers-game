#include <game/checkers_game.h>

#include <iostream>

using namespace std;

int main(int argc, char **argv) {
	try {
		unique_ptr<CheckersGame> game(new CheckersGame());
		game->run();
	} catch (const exception& e) {
		cout << e.what() << endl;
		cin.ignore();

		return 1;
	}
	
	return 0;
}