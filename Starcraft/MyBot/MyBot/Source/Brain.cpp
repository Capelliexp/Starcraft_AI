#include "Brain.h"

//HELPER
/*(X)*/ void DrawBox(Position position, int size) {
	int HBS = size / 2;
	Broodwar->drawBoxMap({ position.x - HBS, position.y - HBS }, { position.x + HBS, position.y + HBS }, 111, true);
}

/*( )*/ void DrawTextOnObject(Unit object, std::string text, int offset) {}

/*(X)*/ void DrawTextOnScreen(std::string text) {
	const char* charText = text.c_str();
	Broodwar->drawTextScreen({ 30,30 }, charText);
}

//BUILDING COMMANDS
/*(X)*/ int BuildBuildingLocation(UnitType building, TilePosition position) {
	for (auto unit : Broodwar->self()->getUnits())
		if (unit->getType().isWorker())
			if (unit->build(building, position)) return 1;

	return 0;
}

/*(X)*/ int TrainUnit(UnitType unitType) {
	std::vector<Unit> vectorList;
	for (auto building : Broodwar->self()->getUnits())
		if (building->getType() == unitType.whatBuilds().first)
			vectorList.push_back(building);
	
	while (vectorList.size() > 0) {
		if (!vectorList.at(vectorList.size() - 1)->isTraining() && !vectorList.at(vectorList.size() - 1)->isResearching()) {
			vectorList.at(vectorList.size() - 1)->train(unitType);
			return 1;
		}
		else vectorList.pop_back();
	}

	return 0;
}

/*(X)*/ int BuildAddOn(Unit building, UnitType addOn) {
	if (building->buildAddon(addOn)) return 1;

	return 0;
}

/*(X)*/ int Research(TechType tech) {
	std::vector<Unit> vectorList;
	for (auto building : Broodwar->self()->getUnits())
		if (building->getType() == tech.whatResearches())
			vectorList.push_back(building);

	while (vectorList.size() > 0) {
		if (!vectorList.at(vectorList.size() - 1)->isTraining() && !vectorList.at(vectorList.size() - 1)->isResearching()) {
			vectorList.at(vectorList.size() - 1)->research(tech);
			return 1;
		}
		else vectorList.pop_back();
	}

	return 0;
}

/*(X)*/ int BuildRefinery(Unit gas) {
	for (auto unit : Broodwar->self()->getUnits())
		if (unit->getType().isWorker())
			if (unit->build(UnitTypes::Enum::Terran_Refinery, gas->getTilePosition())) return 1;

	return 0;
}

//UNIT COMMANDS
/*(X)*/ int MoveUnitDifference(Unit unit, int x, int y, Order moveType = Orders::Move) {
	Position pos = unit->getPosition();
	pos.x += x;
	pos.y += y;
	if (!unit->rightClick(pos)) return 0;
	return 1;
}

/*(X)*/ int OrderUnitLocation(Unit unit, Position position = { 0,0 }, Order command = Orders::Move) {
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

/*(X)*/ int OrderUnitOnUnit(Unit firstUnit, Unit secondUnit, Order command = Orders::Move) {
	switch (command) {
	case Orders::Enum::Move:
		if (!firstUnit->rightClick(secondUnit)) return 0;
		break;
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

/*(X)*/ int OrderUnitCommand(Unit unit, Order command = Orders::Move) {
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
/*(X)*/ std::pair<bool, TilePosition> RepeatSearch(TilePosition coord, bool vert, int i, UnitType building,	bool(*f)(TilePosition pos, UnitType building)) {
	for (int c = -i; c <= i; c++) {
		if (vert == false) {
			if (GoodBuildingSpot({coord.x+c, coord.y}, building)) {
				return { true, { coord.x + c, coord.y } };
			}
		}
		else {
			if (GoodBuildingSpot({coord.x, coord.y+c}, building)) {
				return { true, { coord.x, coord.y + c } };
			}
		}
	}
	return { false, {0,0} };
}

/*(X)*/ bool GoodBuildingSpot(TilePosition pos, UnitType building) {
	if (Broodwar->canBuildHere(pos, building)) {
		TilePosition testPos1 = { pos.x - 1, pos.y - 1 };
		TilePosition testPos2 = { pos.x + 1, pos.y - 1 };
		TilePosition testPos3 = { pos.x - 1, pos.y + 1 };
		TilePosition testPos4 = { pos.x + 1, pos.y + 1 };

		if (Broodwar->canBuildHere(testPos1, building) && Broodwar->canBuildHere(testPos2, building) &&
			Broodwar->canBuildHere(testPos3, building) && Broodwar->canBuildHere(testPos4, building)) {
			return true;
		}
	}
	return false;
}

/*(X)*/ TilePosition FindSuitableBuildingTile(UnitType building, TilePosition origin) {	//place building in open area
	int tileSearchArea = 48;
	std::pair<bool, TilePosition> test;
	for (int i = 2; i < tileSearchArea; i++) {
		test = RepeatSearch({ origin.x - i, origin.y }, false, i, building, GoodBuildingSpot);	//left
		if (test.first == true) return test.second;

		test = RepeatSearch({ origin.x + i, origin.y }, false, i, building, GoodBuildingSpot);	//right
		if (test.first == true) return test.second;

		test = RepeatSearch({ origin.x, origin.y + i }, false, i, building, GoodBuildingSpot);	//up
		if (test.first == true) return test.second;

		test = RepeatSearch({ origin.x, origin.y - i }, false, i, building, GoodBuildingSpot);	//down
		if (test.first == true) return test.second;
	}
	return {1,1};
}

/*(X)*/ Unit FindClosestMineral(Position origin) {
	std::vector<Unit> MineralVector;
	for (auto unit : Broodwar->getMinerals())
		MineralVector.push_back(unit);

	while (MineralVector.size() > 1) {
		if (CloserToOrig(origin, MineralVector.at(0)->getPosition(), MineralVector.at(1)->getPosition()))
			MineralVector.erase(MineralVector.begin() + 1);
		else MineralVector.erase(MineralVector.begin());
	}

	return MineralVector.at(0);
}

/*(X)*/ Unit FindClosestGas(Position origin) {
	std::vector<Unit> GasVector;
	for (auto unit : Broodwar->getGeysers())
		GasVector.push_back(unit);

	while (GasVector.size() > 1) {
		if (CloserToOrig(origin, GasVector.at(0)->getPosition(), GasVector.at(1)->getPosition()))
			GasVector.erase(GasVector.begin() + 1);
		else GasVector.erase(GasVector.begin());
	}

	return GasVector.at(0);
}

/*(X)*/ bool CloserToOrig(Position origin, Position unitAPos, Position unitBPos) {
	double distA = sqrt(pow((abs(unitAPos.x - origin.x)), 2) + pow((abs(unitAPos.y - origin.y)), 2));
	double distB = sqrt(pow((abs(unitBPos.x - origin.x)), 2) + pow((abs(unitBPos.y - origin.y)), 2));

	if (distA < distB) return true;
	return false;
}

//MACRO
/*(X)*/ int IdleWorkersWork(MyBot* bot) {
	for (auto unit : Broodwar->self()->getUnits()) {
		if (unit->getType().isWorker() && unit->isIdle()) {
			Unit mineral = FindClosestMineral(bot->BaseLocations.at(0)->getPosition());
			OrderUnitOnUnit(unit, mineral);
		}
	}
	return 1;
}

/*( )*/ int BaseBuilder() {
	return 1;
}

/*( )*/ int ArmyCreator() {
	return 1;
}

/*( )*/ int Scout() {
	return 1;
}

//MICRO
/*( )*/ int GroupUp() {
	return 1;
}

/*( )*/ int CreateSquad() {
	return 1;
}

/*( )*/ int SquadOrder() {
	return 1;
}

//GLOBAL
/*(X)*/ int CountUnitType(UnitType searchUnit) {
	int count = 0;
	for (auto unit : Broodwar->self()->getUnits()) {
		if (unit->getType() == searchUnit) {
			count++;
		}
	}
	return count;
}

