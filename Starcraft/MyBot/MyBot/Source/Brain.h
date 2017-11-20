#pragma once

#include "MyBot.h"

//HELPER
void DrawBox(Position position);
void DrawTextOnObject(Unit object, std::string text, int offset);

//BUILD
int BuildBuildingLocation(UnitType building, Unit worker, TilePosition position);
int TrainUnit(Unit building, UnitType unit);
int BuildAddOn(Unit building, UnitType addOn);
int Research(Unit building, TechType tech);

//UNIT COMMANDS
int MoveUnitDifference(Unit unit, int x, int y, Order moveType);
int OrderUnitLocation(Unit unit, Position position, Order moveType);
int OrderUnitOnUnit(Unit firstUnit, Unit secondUnit, Order moveType);
int OrderUnitCommand(Unit unit, Order moveType);

//TERRAIN
TilePosition FindSuitableBuildingTile(Position origin);
TilePosition FindClosestMineralTile(Position origin);
TilePosition FindClosestGasTile(Position origin);

Position FindSuitableBuildingPos(Position origin);
Position FindClosestMineralPos(Position origin);
Position FindClosestGasPos(Position origin);

//MACRO
int IdleWorkersWork(MyBot bot);
int BaseBuilder();
int ArmyCreator();
int Scout();

//MICRO
int GroupUp();
int CreateSquad();
int SquadOrder();

//GLOBAL
int CountUnitType(UnitType building);