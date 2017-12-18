#pragma once
#include "MyBot.h"

//HELPER
void DrawBox(Position position, int size, int color = 111);
void DrawTextOnObject(Unit object, std::string text, int offset);
void DrawTextOnScreen(std::string text, int x, int y);

//BUILDING COMMANDS
int BuildBuildingLocation(UnitType building, TilePosition position);
int TrainUnit(UnitType unit);
int BuildAddOn(Unit building, UnitType addOn);
int Research(TechType tech);
int BuildRefinery(Unit gas);

//UNIT COMMANDS
int MoveUnitDifference(Unit unit, int x, int y, Order moveType = Orders::Move);
int OrderUnitLocation(Unit unit, Position position, Order moveType = Orders::Move);
int OrderUnitOnUnit(Unit firstUnit, Unit secondUnit, Order moveType = Orders::Move);
int OrderUnitCommand(Unit unit, Order moveType);

//TERRAIN
std::pair<bool, TilePosition> RepeatSearch(TilePosition coord, bool vert, int i, UnitType building, int space, bool(*f)(TilePosition pos, UnitType building, int reqSpace));
bool GoodBuildingSpot(TilePosition pos, UnitType building, int reqSpace = 1);
TilePosition FindSuitableBuildingTile(UnitType building, TilePosition origin, int reqSpace);
Unit FindClosestMineral(Position origin);
Unit FindClosestGas(Position origin);
bool CloserToOrig(Position origin, Position unitA, Position unitB);
float DistanceBetween(Position posA, Position posB);

//MACRO
int IdleUnits(MyBot* bot);
int IdleWorkersWork(MyBot* bot, Unit unit);
int BaseBuilder();
int ArmyCreator();
int Scout();

//MICRO
int General(MyBot* bot);
int GroupUp(MyBot* bot, int SquadIterator1, int SquadIterator2);
int ChangeSquadOrder(MyBot* bot, int squadIterator, BWAPI::Order command, BWAPI::Position position);
int ChangeSquadOrder(Squad* squadPointer, BWAPI::Order command, BWAPI::Position position);
int AllSquadsExecute(MyBot* bot);
int SingleSquadExecute(MyBot* bot, int squadIterator);
int SingleSquadExecute(MyBot* bot, Squad* squadPointer);
int TankCommander();
int UpdateBaseInfo(MyBot* bot);
bool UnitSeesEnemy(BWAPI::Unit unit);

//GLOBAL
int CountUnitType(UnitType building);