#pragma once

#include "MyBot.h"

//HELPER
void DrawBox(int x, int y);
void DrawTextOnObject(Unit object, std::string text, int offset);

//BUILD
int BuildBuildingCoord(Unit building, int x, int y);
int BuildBuildingLocation(Unit building, Position position);
int BuildUnit(Unit unit, Unit building);
int BuildAddOn(Unit addOn, Unit building);

//MOVE
int MoveUnitDifference(Unit unit, int x, int y, Order moveType);
int MoveUnitCoord(Unit unit, int x, int y, Order moveType);
int MoveUnitLocation(Unit unit, Position position, Order moveType);

//UNIT ABILITY COMMANDS
int TankSiegeMode(Unit unit);

//MAINTENANCE
int IdleWorkersWork();