#include "Brain.h"

//BUILDING COMMANDS
int BuildBuildingLocation(UnitType building, Unit worker, TilePosition position) {
	if (!worker->build(building, position)) return 0;

	return 1;
}

int TrainUnit(Unit building, UnitType unit) {
	if (!building->train(unit)) return 0;

	return 1;
}

int BuildAddOn(Unit building, UnitType addOn) {
	if (!building->buildAddon(addOn)) return 0;

	return 1;
}

int Research(Unit building, TechType tech) {
	if (!building->research(tech)) return 0;

	return 1;
}

//UNIT COMMANDS
int MoveUnitDifference(Unit unit, int x, int y, Order moveType = Orders::Move) {
	Position pos = unit->getPosition();
	pos.x += x;
	pos.y += y;
	if (!unit->rightClick(pos)) return 0;
	return 1;
}

int OrderUnitLocation(Unit unit, Position position = {0,0}, Order command = Orders::Move) {
	switch (command) {
	case Orders::Enum::Move:
		if (!unit->rightClick(position)) return 0;	//OBS! right click
		break;
	case Orders::Enum::AttackUnit:
		if (!unit->attack(position)) return 0;
		break;
	case Orders::Enum::Patrol:
		if (!unit->patrol(position)) return 0;
		break;
	default:
		return -1;
	}

	return 1;
}

int OrderUnitOnUnit(Unit firstUnit, Unit secondUnit, Order command = Orders::Move) {
	switch (command) {
	case Orders::Enum::AttackUnit:
		if (!firstUnit->attack(secondUnit)) return 0;
		break;
	case Orders::Enum::Follow:
		if (!firstUnit->follow(secondUnit)) return 0;
		break;
	case Orders::Enum::Repair:
		if (!firstUnit->repair(secondUnit)) return 0;
		break;
	default:
		return -1;
	}

	return 1;
}

int OrderUnitCommand(Unit unit, Order command = Orders::Move) {
	switch (command) {
	case Orders::Enum::HoldPosition:
		if (!unit->holdPosition()) return 0;
		break;
	case Orders::Enum::Stop:
		if (!unit->stop()) return 0;
		break;
	case Orders::Enum::Burrowing:
		if (!unit->burrow()) return 0;
		break;
	case Orders::Enum::Unburrowing:
		if (!unit->unburrow()) return 0;
		break;
	case Orders::Enum::Cloak:
		if (!unit->cloak()) return 0;
		break;
	case Orders::Enum::Decloak:
		if (!unit->decloak()) return 0;
		break;
	case Orders::Enum::Sieging:
		if (!unit->siege()) return 0;
		break;
	case Orders::Enum::Unsieging:
		if (!unit->unsiege()) return 0;
		break;
	case Orders::Enum::LiftingOff:	//no counterpart?!?
		if (!unit->lift()) return 0;	//counterpart = land(pos)
		break;
	default:
		return -1;
	}

	return 1;
}

//TERRAIN
int RepeatSearch(Position coord, bool vert, int i) {
	for (int c = -i; c <= i; c++) {
		if (vert == false) {
			if (GoodSpot({coord.x+c, coord.y})) {}
		}
		else {
			if (GoodSpot({coord.x, coord.y+c})) {}
		}
	}
}

//canBuildHere (TilePosition position, UnitType type, Unit builder=nullptr, bool checkExplored=false)=0
bool GoodSpot(Position pos) {
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {

		}
	}
}

TilePosition FindSuitableBuildingTile(Position origin) {	//place building in center of free 6x6
	//Position guardPoint = bot->findGuardPoint();
	int tileSearchArea = 48;
	for (int i = 1; i < tileSearchArea; i++) {		
		RepeatSearch({ origin.x - i, origin.y }, false, i);	//left
		RepeatSearch({ origin.x + i, origin.y }, false, i);	//right
		RepeatSearch({ origin.x, origin.y + i }, true, i);	//up
		RepeatSearch({ origin.x, origin.y - i }, true, i);	//down
	}
}

TilePosition FindClosestMineralTile(Position origin) {}

TilePosition FindClosestGasTile(Position origin) {}

Position FindSuitableBuildingPos(Position origin) {}

Position FindClosestMineralPos(Position origin) {}

Position FindClosestGasPos(Position origin) {}

//MACRO
int IdleWorkersWork() {
	for (auto unit : Broodwar->self()->getUnits()) {
		if (unit->getType().isWorker() && unit->isIdle()) {
			OrderUnitLocation(unit, FindClosestMineralPos(bot->basePosition[0]));
		}
	}

}

int BaseBuilder() {}

int ArmyCreator() {}

int Scout() {}

//MICRO
int GroupUp() {}

int CreateSquad() {}

int SquadOrder() {}

//GLOBAL
int CountUnitType(UnitType building) {}