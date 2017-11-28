#pragma once
#include "MyBot.h"

//HELPER
void DrawBox(Position position, int size);
void DrawTextOnObject(Unit object, std::string text, int offset);
void DrawTextOnScreen(std::string text);
//BUILD
int BuildBuildingLocation(UnitType building, TilePosition position);
int TrainUnit(UnitType unit);
int BuildAddOn(Unit building, UnitType addOn);
int Research(TechType tech);

//UNIT COMMANDS
int MoveUnitDifference(Unit unit, int x, int y, Order moveType);
int OrderUnitLocation(Unit unit, Position position, Order moveType);
int OrderUnitOnUnit(Unit firstUnit, Unit secondUnit, Order moveType);
int OrderUnitCommand(Unit unit, Order moveType);

//TERRAIN
std::pair<bool, TilePosition> RepeatSearch(TilePosition coord, bool vert, int i, UnitType building, bool(*f)(TilePosition pos, UnitType building));
bool GoodBuildingSpot(TilePosition pos, UnitType building);

TilePosition FindSuitableBuildingTile(UnitType building, TilePosition origin);
TilePosition FindClosestMineralTile(Position origin);
TilePosition FindClosestGasTile(Position origin);

bool CloserToOrig(Position origin, Position unitA, Position unitB);

//Position FindSuitableBuildingPos(Position origin);
//Position FindClosestMineralPos(TilePosition base);
//Position FindClosestGasPos(TilePosition base);

//MACRO
int IdleWorkersWork(MyBot* bot);
int BaseBuilder();
int ArmyCreator();
int Scout();

//MICRO
int GroupUp();
int CreateSquad();
int SquadOrder();

//GLOBAL
int CountUnitType(UnitType building);