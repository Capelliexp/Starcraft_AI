#include "Brain.h"

//HELPER
/*(X)*/ void DrawBox(Position position, int size, int color) {
	int HBS = size / 2;
	Broodwar->drawBoxMap({ position.x - HBS, position.y - HBS }, { position.x + HBS, position.y + HBS }, 111, true);
}

/*( )*/ void DrawTextOnObject(Unit object, std::string text, int offset) {}

/*(X)*/ void DrawTextOnScreen(std::string text, int x, int y) {
	const char* charText = text.c_str();
	Broodwar->drawTextScreen({ x,y }, charText);
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
/*(X)*/ int MoveUnitDifference(Unit unit, int x, int y, Order moveType) {
	Position pos = unit->getPosition();
	pos.x += x;
	pos.y += y;
	if (!unit->rightClick(pos)) return 0;
	return 1;
}

/*(X)*/ int OrderUnitLocation(Unit unit, Position position, Order command) {
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
	case Orders::Enum::AttackMove:
		if (!unit->attack(position)) return 0;
		break;
	default:
		return -1;
	}

	return 1;
}

/*(X)*/ int OrderUnitOnUnit(Unit firstUnit, Unit secondUnit, Order command) {
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
	case Orders::Enum::HarvestGas:
		if (!firstUnit->gather(secondUnit)) return 0;
		break;
	case Orders::Enum::MoveToMinerals:
		if (!firstUnit->gather(secondUnit)) return 0;
		break;
	default:
		return 0;
	}

	return 1;
}

/*(X)*/ int OrderUnitCommand(Unit unit, Order command) {
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
/*(X)*/ std::pair<bool, TilePosition> RepeatSearch(TilePosition coord, bool vert, int i, UnitType building, int space, bool(*f)(TilePosition pos, UnitType building, int reqSpace)) {
	for (int c = -i; c <= i; c++) {
		if (vert == false) {
			if (f({coord.x+c, coord.y}, building, space)) {
				return { true, { coord.x + c, coord.y } };
			}
		}
		else {
			if (f({coord.x, coord.y+c}, building, space)) {
				return { true, { coord.x, coord.y + c } };
			}
		}
	}
	return { false, {0,0} };
}

/*(X)*/ bool GoodBuildingSpot(TilePosition pos, UnitType building, int reqSpace) {
	if (Broodwar->canBuildHere(pos, building)) {
		TilePosition testPos1 = { pos.x - reqSpace, pos.y - reqSpace };
		TilePosition testPos2 = { pos.x + reqSpace, pos.y - reqSpace };
		TilePosition testPos3 = { pos.x - reqSpace, pos.y + reqSpace };
		TilePosition testPos4 = { pos.x + reqSpace, pos.y + reqSpace };

		if (Broodwar->canBuildHere(testPos1, building) && Broodwar->canBuildHere(testPos2, building) &&
			Broodwar->canBuildHere(testPos3, building) && Broodwar->canBuildHere(testPos4, building)) {
			return true;
		}
	}
	return false;
}

/*(X)*/ TilePosition FindSuitableBuildingTile(UnitType building, TilePosition origin, int reqSpace) {	//place building in open area
	int tileSearchArea = 48;
	std::pair<bool, TilePosition> test;
	for (int i = 2; i < tileSearchArea; i++) {
		test = RepeatSearch({ origin.x - i, origin.y }, false, i, building, reqSpace, GoodBuildingSpot);	//left
		if (test.first == true) return test.second;

		test = RepeatSearch({ origin.x + i, origin.y }, false, i, building, reqSpace, GoodBuildingSpot);	//right
		if (test.first == true) return test.second;

		test = RepeatSearch({ origin.x, origin.y + i }, false, i, building, reqSpace, GoodBuildingSpot);	//up
		if (test.first == true) return test.second;

		test = RepeatSearch({ origin.x, origin.y - i }, false, i, building, reqSpace, GoodBuildingSpot);	//down
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

/*(X)*/ Unit FindClosestRefinery(Position origin) {
	std::vector<Unit> RefVector;
	for (auto unit : Broodwar->self()->getUnits())
		if (unit->getType().getID() == UnitTypes::Enum::Terran_Refinery) {
			RefVector.push_back(unit);
		}

	while (RefVector.size() > 1) {
		if (CloserToOrig(origin, RefVector.at(0)->getPosition(), RefVector.at(1)->getPosition()))
			RefVector.erase(RefVector.begin() + 1);
		else RefVector.erase(RefVector.begin());
	}

	return RefVector.at(0);
}

/*(X)*/ bool CloserToOrig(Position origin, Position unitAPos, Position unitBPos) {
	double distA = sqrt(pow((abs(unitAPos.x - origin.x)), 2) + pow((abs(unitAPos.y - origin.y)), 2));
	double distB = sqrt(pow((abs(unitBPos.x - origin.x)), 2) + pow((abs(unitBPos.y - origin.y)), 2));

	if (distA < distB) return true;
	return false;
}

//MACRO
/*(X)*/ int IdleUnits(MyBot* bot) {
	for (auto unit : Broodwar->self()->getUnits()) {
		if (unit->isIdle()) {
			if (unit->getType().isWorker()) {	//worker
				if (!IdleWorkersWork(bot, unit)) return 0;
			}
			//else 
			else if (unit->getType() == UnitTypes::Enum::Terran_Machine_Shop)	//Machine Shop
				unit->research(TechTypes::Enum::Tank_Siege_Mode);
			//...
		}
	}
	return 1;
}

/*(X)*/ int IdleWorkersWork(MyBot* bot, Unit unit) {
	int miners = 0;
	int gassers = 0;
	for (auto unit : Broodwar->self()->getUnits()) {
		if (unit->getType().getID() == UnitTypes::Enum::Terran_SCV) {
			if (unit->isGatheringMinerals()) miners++;
			else if (unit->isGatheringGas()) gassers++;
		}
	}

	if (bot->currentSubTaskNr > 4 && gassers < 2) {
		if(OrderUnitOnUnit(unit, FindClosestRefinery(unit->getPosition()), Orders::HarvestGas)) return 1;
		
	}
	else {
		if(OrderUnitOnUnit(unit, FindClosestMineral(unit->getPosition()), Orders::MoveToMinerals)) return 1;
	}

	return 0;
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
/*( )*/ int General(MyBot* bot) {
	TankCommander();

	if (bot->currentSubTaskNr > 12) {
		Attack(bot);
	}

	return 1;
}

/*( )*/ int GroupUp() {
	return 1;
}

/*( )*/ int CreateSquad() {
	return 1;
}

/*( )*/ int SquadOrder() {
	return 1;
}

/*( )*/ int Attack(MyBot* bot) {
	Position attackPos = bot->EnemyBases.at(0)->getPosition();
	for (auto unit : Broodwar->self()->getUnits()) {
		UnitType type = unit->getType();
		if (type.getID() < 100 && type.getID() != UnitTypes::Enum::Terran_SCV) {
			OrderUnitLocation(unit, attackPos, Orders::AttackMove);
			//Broodwar->sendText("ATTACKING!");
		}
	}
	return 1;
}

/*(X)*/ int TankCommander() {
	std::vector<BWAPI::Unit> tank_normal;
	std::vector<BWAPI::Unit> tank_siege;

	for (auto unit : Broodwar->self()->getUnits()) {	//get all tanks...
		if (unit->getType() == UnitTypes::Enum::Terran_Siege_Tank_Tank_Mode)	//...in normal mode
			tank_normal.push_back(unit);
		else if (unit->getType() == UnitTypes::Enum::Terran_Siege_Tank_Siege_Mode)	//...in siege mode
			tank_siege.push_back(unit);
	}

	//---

	for (int i = 0; i < tank_normal.size(); i++)
		for (auto enemyUnit : Broodwar->enemy()->getUnits())
			if (tank_normal.at(i)->isInWeaponRange(enemyUnit)) {
				OrderUnitCommand(tank_normal.at(i), Orders::Enum::Sieging);
				break;
			}

	for (int i = 0; i < tank_siege.size(); i++) {
		bool chillOutMan = true;
		for (auto enemyUnit : Broodwar->enemy()->getUnits())
			if (tank_siege.at(i)->isSieged())
				if (tank_siege.at(i)->isInWeaponRange(enemyUnit)) {
					chillOutMan = false;
					break;
				}
		if (chillOutMan == true)
			if (tank_siege.at(i)->isIdle())
				OrderUnitCommand(tank_siege.at(i), Orders::Enum::Unsieging);
	}

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

