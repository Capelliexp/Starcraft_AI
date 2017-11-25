#include "Brain.h"

void DrawBox(Position position) {
	BWAPI::Game::drawBoxScreen({position.x-5, position.y+5}, {position.x+5, position.y-5}, 111, true);
}

void DrawTextOnObject(Unit object, std::string text, int offset) {}