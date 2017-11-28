//#include "MyBot.h"
#include "Brain.h"

using namespace BWAPI;

bool analyzed;
bool analysis_just_finished;
BWTA::Region* home;
BWTA::Region* enemy_base;

//This is the startup method. It is called once
//when a new game has been started with the bot.
void MyBot::onStart()
{
	frameCount10 = 0;
	frameCount100 = 0;
	frameCount1000 = 0;
	task = new subTask[12];
	task[0] =  { 1, 1, UnitTypes::Enum::Terran_Supply_Depot, 2, 0, 0 };
	task[1] =  { 1, 2, UnitTypes::Enum::Terran_Barracks, 1, 0, 0 };
	task[2] =  { 1, 3, UnitTypes::Enum::Terran_Marine, 10, 0, 0 };
	task[3] =  { 2, 1, UnitTypes::Enum::Terran_Academy, 1, 0, 0 };
	task[4] =  { 2, 2, UnitTypes::Enum::Terran_Refinery, 1, 0, 0 };
	task[5] =  { 2, 3, UnitTypes::Enum::Terran_Medic, 3, 0, 0 };
	task[6] =  { 2, 4, UnitTypes::Enum::Terran_SCV, 5, 0, 0 };
	task[7] =  { 3, 1, UnitTypes::Enum::Terran_Factory, 1, 0, 0 };
	task[8] =  { 3, 2, UnitTypes::Enum::Terran_Machine_Shop, 1, 0, 0 };
	task[9] =  { 3, 3, UnitTypes::Enum::Terran_Siege_Tank_Tank_Mode, 3, 0, 0 };
	task[10] = { 4, 1, UnitTypes::Enum::Terran_Command_Center, 1, 0, 0 };
	task[11] = { 4, 2, UnitTypes::Enum::Terran_SCV, 4, 0, 0 };
	currentSubTask = task[0];
	currentSubTaskNr = 0;

	//baseTile = new TilePosition[1];
	for (auto unit : Broodwar->self()->getUnits())
		if (unit->getType() == UnitTypes::Enum::Terran_Command_Center)
			baseTile.push_back(unit->getTilePosition());

	//basePosition = new Position[1];
	for (auto unit : Broodwar->self()->getUnits())
		if (unit->getType() == UnitTypes::Enum::Terran_Command_Center)
			basePosition.push_back(unit->getPosition());

	Broodwar->sendText("Bot start");
	//Enable flags
	Broodwar->enableFlag(Flag::UserInput);
	//Uncomment to enable complete map information
	//Broodwar->enableFlag(Flag::CompleteMapInformation);

	//Start analyzing map data
	BWTA::readMap();
	analyzed = false;
	analysis_just_finished = false;
	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL); //Threaded version
	AnalyzeThread();

	//Send each worker to the mineral field that is closest to it
	for (auto u : Broodwar->self()->getUnits())
	{
		if (u->getType().isWorker())
		{
			Unit closestMineral = NULL;
			for (auto m : Broodwar->getMinerals())
			{
				if (closestMineral == NULL || u->getDistance(m) < u->getDistance(closestMineral))
				{
					closestMineral = m;
				}
			}
			if (closestMineral != NULL)
			{
				u->rightClick(closestMineral);
				//Broodwar->printf("Send worker %d to mineral %d", u->getID(), closestMineral->getID());
			}
		}
	}
}

//This is the method called each frame. This is where the bot's logic
//shall be called.
void MyBot::onFrame() {
	frameCount10++;
	frameCount100++;
	frameCount1000++;

	//Draw lines around regions, chokepoints etc.
	if (analyzed) drawTerrainData();

	//Draw box on all friendly units
	for (auto unit : Broodwar->self()->getUnits()) DrawBox(unit->getPosition(), 5);

	//Print task
	std::string a = 
		"Current Task:\n   task " + std::to_string(currentSubTask.task) + "." + std::to_string(currentSubTask.step) +
		"\n   constructing " + currentSubTask.construct.getName() + "\n   " +
		std::to_string(currentSubTask.requiredUnits) + ", " + std::to_string(currentSubTask.completedUnits) + ", " +
		std::to_string(currentSubTask.inProgressUnits);
	DrawTextOnScreen(a);	

	//Follow task
	if (frameCount100 == 100) {
		Broodwar->sendText("Unit create");
		if ((currentSubTask.completedUnits + currentSubTask.inProgressUnits) < currentSubTask.requiredUnits) {
			if (currentSubTask.construct > 100 /* == UnitTypes::Buildings*/) {
				if (BuildBuildingLocation(currentSubTask.construct, FindSuitableBuildingTile(currentSubTask.construct, baseTile.at(0))))
					currentSubTask.inProgressUnits++;
				else
					Broodwar->sendText("   Failed to build building");
			}
			else if (currentSubTask.construct < 100 /* == UnitTypes::AllUnits*/) {
				if (TrainUnit(currentSubTask.construct))
					currentSubTask.inProgressUnits++;
				else 
					Broodwar->sendText("   Failed to train unit");
			}
			else Broodwar->sendText("   Failed to interprete unit type");
		}
		else if (currentSubTask.inProgressUnits == 0 && CountUnitType(currentSubTask.construct) == currentSubTask.requiredUnits) {
			currentSubTaskNr++;
			currentSubTask = task[currentSubTaskNr];
			Broodwar->sendText("   Getting new task");
		}
		else if (currentSubTask.inProgressUnits > 0) Broodwar->sendText("   Current production not done");
		else Broodwar->sendText("   Failed to start new task");
	}

	IdleWorkersWork(this);

	/*if (frameCount100 == 100){
		frameCount100 = 0;
		//Order one of our workers to guard our chokepoint.
		//Iterate through the list of units.
		for (auto unit : Broodwar->self()->getUnits()){
			//Check if unit is a worker.
			if (unit->getType().isWorker()){
				//Find guard point
				Position guardPoint = findGuardPoint();
				//Order the worker to move to the guard point
				unit->rightClick(guardPoint);
				//Only send the first worker.
				break;
			}
		}
	}*/

	if (frameCount10 == 10) frameCount10 = 0;
	if (frameCount100 == 100) frameCount100 = 0;
	if (frameCount1000 == 1000) frameCount1000 = 0;
}

//Called when a game is ended.
//No need to change this.
void MyBot::onEnd(bool isWinner)
{
	if (isWinner) Broodwar->sendText("WINNER!");
	else Broodwar->sendText("loser");
}

//Finds a guard point around the home base.
//A guard point is the center of a chokepoint surrounding
//the region containing the home base.
Position MyBot::findGuardPoint()
{
	//Get the chokepoints linked to our home region
	std::set<BWTA::Chokepoint*> chokepoints = home->getChokepoints();
	double min_length = 10000;
	BWTA::Chokepoint* choke = NULL;

	//Iterate through all chokepoints and look for the one with the smallest gap (least width)
	for (std::set<BWTA::Chokepoint*>::iterator c = chokepoints.begin(); c != chokepoints.end(); c++)
	{
		double length = (*c)->getWidth();
		if (length < min_length || choke == NULL)
		{
			min_length = length;
			choke = *c;
		}
	}

	return choke->getCenter();
}

//Is called when text is written in the console window.
//Can be used to toggle stuff on and off.
void MyBot::onSendText(std::string text)
{
	if (text == "/show players")
	{
		showPlayers();
	}
	else if (text == "/show forces")
	{
		showForces();
	}
	else
	{
		Broodwar->printf("You typed '%s'!", text.c_str());
		Broodwar->sendText("%s", text.c_str());
	}
}

//Called when the opponent sends text messages.
//No need to change this.
void MyBot::onReceiveText(BWAPI::Player player, std::string text)
{
	Broodwar->printf("%s said '%s'", player->getName().c_str(), text.c_str());
}

//Called when a player leaves the game.
//No need to change this.
void MyBot::onPlayerLeft(BWAPI::Player player)
{
	Broodwar->sendText("%s left the game.", player->getName().c_str());
}

//Called when a nuclear launch is detected.
//No need to change this.
void MyBot::onNukeDetect(BWAPI::Position target)
{
	if (target != Positions::Unknown)
	{
		Broodwar->printf("Nuclear Launch Detected at (%d,%d)", target.x, target.y);
	}
	else
	{
		Broodwar->printf("Nuclear Launch Detected");
	}
}

//No need to change this.
void MyBot::onUnitDiscover(BWAPI::Unit unit)
{
	//Broodwar->sendText("A %s [%x] has been discovered at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x,unit->getPosition().y);
}

//No need to change this.
void MyBot::onUnitEvade(BWAPI::Unit unit)
{
	//Broodwar->sendText("A %s [%x] was last accessible at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x,unit->getPosition().y);
}

//No need to change this.
void MyBot::onUnitShow(BWAPI::Unit unit)
{
	//Broodwar->sendText("A %s [%x] has been spotted at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x,unit->getPosition().y);
}

//No need to change this.
void MyBot::onUnitHide(BWAPI::Unit unit)
{
	//Broodwar->sendText("A %s [%x] was last seen at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x,unit->getPosition().y);
}

//Called when a new unit has been created.
//Note: The event is called when the new unit is built, not when it
//has been finished.
void MyBot::onUnitCreate(BWAPI::Unit unit)
{
	if (unit->getPlayer() == Broodwar->self())
	{
		Broodwar->sendText("A %s [%x] has been created at (%d,%d)", unit->getType().getName().c_str(), unit, unit->getPosition().x, unit->getPosition().y);
	}
}

//Called when a unit has been destroyed.
void MyBot::onUnitDestroy(BWAPI::Unit unit)
{
	if (unit->getPlayer() == Broodwar->self())
	{
		Broodwar->sendText("My unit %s [%x] has been destroyed at (%d,%d)", unit->getType().getName().c_str(), unit, unit->getPosition().x, unit->getPosition().y);
	}
	else
	{
		Broodwar->sendText("Enemy unit %s [%x] has been destroyed at (%d,%d)", unit->getType().getName().c_str(), unit, unit->getPosition().x, unit->getPosition().y);
	}
}

//Only needed for Zerg units.
//No need to change this.
void MyBot::onUnitMorph(BWAPI::Unit unit)
{
	//Broodwar->sendText("A %s [%x] has been morphed at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x,unit->getPosition().y);
}

//No need to change this.
void MyBot::onUnitRenegade(BWAPI::Unit unit)
{
	//Broodwar->sendText("A %s [%x] is now owned by %s",unit->getType().getName().c_str(),unit,unit->getPlayer()->getName().c_str());
}

//No need to change this.
void MyBot::onSaveGame(std::string gameName)
{
	Broodwar->printf("The game was saved to \"%s\".", gameName.c_str());
}

//Analyzes the map.
//No need to change this.
DWORD WINAPI AnalyzeThread()
{
	BWTA::analyze();

	//Self start location only available if the map has base locations
	if (BWTA::getStartLocation(BWAPI::Broodwar->self()) != NULL)
	{
		home = BWTA::getStartLocation(BWAPI::Broodwar->self())->getRegion();
	}
	//Enemy start location only available if Complete Map Information is enabled.
	if (BWTA::getStartLocation(BWAPI::Broodwar->enemy()) != NULL)
	{
		enemy_base = BWTA::getStartLocation(BWAPI::Broodwar->enemy())->getRegion();
	}
	analyzed = true;
	analysis_just_finished = true;
	return 0;
}

//Prints some stats about the units the player has.
//No need to change this.
void MyBot::drawStats()
{
	BWAPI::Unitset myUnits = Broodwar->self()->getUnits();
	Broodwar->drawTextScreen(5, 0, "I have %d units:", myUnits.size());
	std::map<UnitType, int> unitTypeCounts;
	for (auto u : myUnits)
	{
		if (unitTypeCounts.find(u->getType()) == unitTypeCounts.end())
		{
			unitTypeCounts.insert(std::make_pair(u->getType(), 0));
		}
		unitTypeCounts.find(u->getType())->second++;
	}
	int line = 1;
	for (std::map<UnitType, int>::iterator i = unitTypeCounts.begin(); i != unitTypeCounts.end(); i++)
	{
		Broodwar->drawTextScreen(5, 16 * line, "- %d %ss", i->second, i->first.getName().c_str());
		line++;
	}
}

//Draws terrain data aroung regions and chokepoints.
//No need to change this.
void MyBot::drawTerrainData()
{
	//Iterate through all the base locations, and draw their outlines.
	for (auto bl : BWTA::getBaseLocations())
	{
		TilePosition p = bl->getTilePosition();
		Position c = bl->getPosition();
		//Draw outline of center location
		Broodwar->drawBox(CoordinateType::Map, p.x * 32, p.y * 32, p.x * 32 + 4 * 32, p.y * 32 + 3 * 32, Colors::Blue, false);
		//Draw a circle at each mineral patch
		for (auto m : bl->getStaticMinerals())
		{
			Position q = m->getInitialPosition();
			Broodwar->drawCircle(CoordinateType::Map, q.x, q.y, 30, Colors::Cyan, false);
		}
		//Draw the outlines of vespene geysers
		for (auto v : bl->getGeysers())
		{
			TilePosition q = v->getInitialTilePosition();
			Broodwar->drawBox(CoordinateType::Map, q.x * 32, q.y * 32, q.x * 32 + 4 * 32, q.y * 32 + 2 * 32, Colors::Orange, false);
		}
		//If this is an island expansion, draw a yellow circle around the base location
		if (bl->isIsland())
		{
			Broodwar->drawCircle(CoordinateType::Map, c.x, c.y, 80, Colors::Yellow, false);
		}
	}
	//Iterate through all the regions and draw the polygon outline of it in green.
	for (auto r : BWTA::getRegions())
	{
		BWTA::Polygon p = r->getPolygon();
		for (int j = 0; j<(int)p.size(); j++)
		{
			Position point1 = p[j];
			Position point2 = p[(j + 1) % p.size()];
			Broodwar->drawLine(CoordinateType::Map, point1.x, point1.y, point2.x, point2.y, Colors::Green);
		}
	}
	//Visualize the chokepoints with red lines
	for (auto r : BWTA::getRegions())
	{
		for (auto c : r->getChokepoints())
		{
			Position point1 = c->getSides().first;
			Position point2 = c->getSides().second;
			Broodwar->drawLine(CoordinateType::Map, point1.x, point1.y, point2.x, point2.y, Colors::Red);
		}
	}
}

//Show player information.
//No need to change this.
void MyBot::showPlayers()
{
	for (auto p : Broodwar->getPlayers())
	{
		Broodwar->printf("Player [%d]: %s is in force: %s", p->getID(), p->getName().c_str(), p->getForce()->getName().c_str());
	}
}

//Show forces information.
//No need to change this.
void MyBot::showForces()
{
	for (auto f : Broodwar->getForces())
	{
		BWAPI::Playerset players = f->getPlayers();
		Broodwar->printf("Force %s has the following players:", f->getName().c_str());
		for (auto p : players)
		{
			Broodwar->printf("  - Player [%d]: %s", p->getID(), p->getName().c_str());
		}
	}
}

//Called when a unit has been completed, i.e. finished built.
void MyBot::onUnitComplete(BWAPI::Unit unit)
{
	Broodwar->sendText("A %s [%x] has been completed at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x,unit->getPosition().y);
	if (unit->getType() == currentSubTask.construct) {
		currentSubTask.completedUnits++;
		currentSubTask.inProgressUnits--;
	}
}
