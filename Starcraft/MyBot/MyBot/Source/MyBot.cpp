//#include "MyBot.h"
#include "Brain.h"

using namespace BWAPI;

bool analyzed;
bool analysis_just_finished;
BWTA::Region* home;
BWTA::Region* enemy_base;

//This is the startup method. It is called once
//when a new game has been started with the bot.
void MyBot::onStart(){
	Broodwar->sendText("Bot start");

	frameCount10 = 1;
	frameCount100 = 2;
	frameCount1000 = 3;
	sanityCount = 0;

	//strats: http://korhal.info.pl/#/korhal (Flash/Last/Sea)

	task = new subTask[14];
	task[0] =  { 1, 1, 0, UnitTypes::Enum::Terran_Supply_Depot, 2, 0, 0 };
	task[1] =  { 1, 2, 0, UnitTypes::Enum::Terran_Barracks, 1, 0, 0 };
	task[2] =  { 1, 3, 0, UnitTypes::Enum::Terran_Marine, 10, 0, 0 };
	task[3] =  { 2, 1, 0, UnitTypes::Enum::Terran_Academy, 1, 0, 0 };
	task[4] =  { 2, 2, 0, UnitTypes::Enum::Terran_Refinery, 1, 0, 0 };
	task[5] =  { 2, 3, 0, UnitTypes::Enum::Terran_Medic, 3, 0, 0 };
	task[6] =  { 2, 4, 0, UnitTypes::Enum::Terran_SCV, 5, 0, 0 };
	task[7] =  { 3, 1, 0, UnitTypes::Enum::Terran_Factory, 1, 0, 0 };
	task[8] =  { 3, 2, 0, UnitTypes::Enum::Terran_Machine_Shop, 1, 0, 0 };
	task[9] =  { 3, 3, 0, UnitTypes::Enum::Terran_Supply_Depot, 2, 0, 0 };
	task[10] = { 3, 4, 0, UnitTypes::Enum::Terran_Siege_Tank_Tank_Mode, 3, 0, 0 };
	task[11] = { 4, 1, 1, UnitTypes::Enum::Terran_Command_Center, 1, 0, 0 };
	task[12] = { 4, 2, 1, UnitTypes::Enum::Terran_SCV, 4, 0, 0 };
	task[13] = { 0, 0, 0, UnitTypes::Enum::None, 0, 0, 0 };
	currentSubTask = task[0];
	currentSubTaskNr = 0;

	recruitmentSquad = nullptr;

	Broodwar->enableFlag(Flag::UserInput);
	Broodwar->setLocalSpeed(5);	//THE NEED FOR SPEED
	Broodwar->setFrameSkip(4);	//SUPER DUPER SPEED
	//Broodwar->enableFlag(Flag::CompleteMapInformation);	//complete map information
	Broodwar->sendText("power overwhelming");

	//Start analyzing map data
	BWTA::readMap();
	analyzed = false;
	analysis_just_finished = false;
	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL); //Threaded version
	AnalyzeThread();

	for (auto a : home->getBaseLocations()) {	//my base
		this->CloseBaseLocations.push_back(a);
	}

	std::set<BaseLocation*> AllBases = BWTA::getBaseLocations();
	for (auto baseLoc : BWTA::getBaseLocations()) {
		if (baseLoc->getPosition() != CloseBaseLocations.at(0)->getPosition()) {
			PotentialEnemyBasePositions.push_back(baseLoc->getPosition());
			AllBasePositions.push_back(baseLoc->getPosition());
		}
	}

	BaseLocation* closest = *AllBases.begin();
	BaseLocation* farthest = *AllBases.begin();

	for (unsigned int i = 1; i < AllBases.size(); i++) {
		BaseLocation* contender = *AllBases.begin();
		
		if (CloserToOrig(CloseBaseLocations.at(0)->getPosition(), contender->getPosition(), farthest->getPosition()))
			farthest = contender;
		if (CloserToOrig(CloseBaseLocations.at(0)->getPosition(), closest->getPosition(), contender->getPosition()))
			closest = contender;
		AllBases.erase(0);
	}
	CloseBaseLocations.push_back(closest);
	EnemyBases.push_back(farthest);
	
	for (auto choke : home->getChokepoints()) {
		armyGroupPoint = choke->getCenter();
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
	//for (auto unit : Broodwar->self()->getUnits()) DrawBox(unit->getPosition(), 5);
	//for(auto base : BWTA::getBaseLocations()) DrawBox(base->getPosition(), 10, 222);
	/*for (int i = 0; i < CloseBaseLocations.size(); i++) {
		DrawBox(CloseBaseLocations.at(i)->getPosition(), 10, 222);
		Broodwar->sendText("%d", i);
	}*/
	/*if (frameCount100 == 100) {
		for (int i = 0; i < Squads.size(); i++) {
			Broodwar->sendText("Order: %d", Squads.at(i)->command);
			Broodwar->sendText("Pos: (%d, %d)", Squads.at(i)->position.x, Squads.at(i)->position.y);
		}
	}*/

	//Print task
	std::string a = 
		"Current Task:\n   task " + std::to_string(currentSubTaskNr) + " - " + std::to_string(currentSubTask.task) + "." + std::to_string(currentSubTask.step) +
		"\n   constructing " + currentSubTask.construct.getName() + " at base " + std::to_string(currentSubTask.base) + "\n   " +
		std::to_string(currentSubTask.requiredUnits) + ", " + std::to_string(currentSubTask.completedUnits) + ", " +
		std::to_string(currentSubTask.inProgressUnits);
	DrawTextOnScreen(a, 10, 10);

	std::string b = "          Close bases: " + std::to_string(CloseBaseLocations.size()) + 
		          "\n         Enemy Bases: " + std::to_string(EnemyBases.size()) + 
		          "\nPotential Enemy Bases: " + std::to_string(PotentialEnemyBasePositions.size()) +
		          "\n\n         Army Squads: " + std::to_string(Squads.size());
	DrawTextOnScreen(b, 480, 20);

	//Follow task
	if (frameCount100 == 100 && currentSubTaskNr < 13) {
		if ((currentSubTask.completedUnits + currentSubTask.inProgressUnits) < currentSubTask.requiredUnits) {
			if (currentSubTask.construct > 100 /* == UnitTypes::Buildings*/) {
				if (currentSubTask.construct == UnitTypes::Enum::Terran_Refinery) {
					if (BuildRefinery(FindClosestGas(CloseBaseLocations.at(currentSubTask.base)->getPosition()))) {
						currentSubTask.inProgressUnits++;
						//Broodwar->sendText("Building refinery");
					}
					else { /*Broodwar->sendText("Failed to build refinery"); */ }
				}
				else if (currentSubTask.construct == UnitTypes::Enum::Terran_Comsat_Station ||
					currentSubTask.construct == UnitTypes::Enum::Terran_Control_Tower ||
					currentSubTask.construct == UnitTypes::Enum::Terran_Covert_Ops ||
					currentSubTask.construct == UnitTypes::Enum::Terran_Machine_Shop ||
					currentSubTask.construct == UnitTypes::Enum::Terran_Nuclear_Silo ||
					currentSubTask.construct == UnitTypes::Enum::Terran_Physics_Lab) {
					for (auto unit : Broodwar->self()->getUnits()) {
						if (unit->canBuildAddon(currentSubTask.construct)) {
							if (BuildAddOn(unit, currentSubTask.construct)) {
								currentSubTask.inProgressUnits++;
								//Broodwar->sendText("Building addon");
							}
							else { /*Broodwar->sendText("Failed to build addon"); */ }
						}
					}
				}
				else if (BuildBuildingLocation(currentSubTask.construct, FindSuitableBuildingTile(currentSubTask.construct, CloseBaseLocations.at(currentSubTask.base)->getTilePosition(), 2))){
					currentSubTask.inProgressUnits++;
					//Broodwar->sendText("Building building");
				}
				else { /*Broodwar->sendText("Failed to build building"); */ }
			}
			else if (currentSubTask.construct < 100 /* == UnitTypes::AllUnits*/) {
				if (TrainUnit(currentSubTask.construct)) {
					currentSubTask.inProgressUnits++;
					//Broodwar->sendText("Creating unit");
				}
				else { /*Broodwar->sendText("Failed to train unit"); */ }
			}
			else {
				Broodwar->sendText("Failed to interprete unit type");
			}
		}
		else if (currentSubTask.completedUnits == currentSubTask.requiredUnits) {
			currentSubTaskNr++;
			sanityCount = 0;
			currentSubTask = task[currentSubTaskNr];
			Broodwar->sendText("Getting new task");
		}
		else if (currentSubTask.inProgressUnits > 0) {	//sanity check - will fuck up if recources are short
			int constructionCounter = 0;
			int workersWorkingCounter = 0;
			for (auto unit : Broodwar->self()->getUnits()) {
				if (unit->getType() == currentSubTask.construct) constructionCounter++;
				else if (unit->getType().isWorker() && unit->isConstructing()) workersWorkingCounter++;
			}
			if ((constructionCounter + workersWorkingCounter) < currentSubTask.requiredUnits)
				currentSubTask.inProgressUnits--;
			else {
				sanityCount++;
				//Broodwar->sendText("Current production not done");
				if (sanityCount > 20) {
					currentSubTask.inProgressUnits--;
					sanityCount = 0;
				}
			}
		}
		else Broodwar->sendText("Failed to start new task");
	}

	if (frameCount100 == 100) IdleUnits(this);
	if (frameCount10 == 10) General(this);

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
		//Broodwar->sendText("A %s [%x] has been created at (%d,%d)", unit->getType().getName().c_str(), unit, unit->getPosition().x, unit->getPosition().y);
	}
}

//Called when a unit has been destroyed.
void MyBot::onUnitDestroy(BWAPI::Unit unit){
	if (unit->getPlayer() == Broodwar->self()) {
		//Broodwar->sendText("My unit %s [%x] has been destroyed at (%d,%d)", unit->getType().getName().c_str(), unit, unit->getPosition().x, unit->getPosition().y);
		for (int i = 0; i < Squads.size(); i++) {
			for (int j = 0; j < Squads.at(i)->members.size(); j++) {
				if (Squads.at(i)->members.at(j) == unit) {
					Squads.at(i)->members.erase(Squads.at(i)->members.begin() + j);
					Broodwar->sendText("RIP unit");
					goto end;
				}
			}
		}
	end:;
	}
	else {
		//Broodwar->sendText("Enemy unit %s [%x] has been destroyed at (%d,%d)", unit->getType().getName().c_str(), unit, unit->getPosition().x, unit->getPosition().y);
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
void MyBot::onUnitComplete(BWAPI::Unit unit){
	if (unit->getPlayer() == Broodwar->self()) {
		//Broodwar->sendText("A %s [%x] has been completed at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x,unit->getPosition().y);
		if (unit->getType() == currentSubTask.construct) {
			currentSubTask.completedUnits++;
			currentSubTask.inProgressUnits--;
		}
		if (unit->getType().getID() != (int)UnitTypes::Enum::Terran_SCV) {
			//Broodwar->sendText("unit created with id %d", unit->getType().getID());
			if (unit->getType().getID() < 100) {
				if (recruitmentSquad != nullptr) {
					recruitmentSquad->members.push_back(unit);
					//Broodwar->sendText("A unit was added to the recruitment squad, new size: %d", recruitmentSquad->members.size());
				}
				else {
					Squad* temp = new Squad();
					temp->members.push_back(unit);
					temp->command = BWAPI::Orders::None;
					temp->position = { 0, 0 };
					Squads.push_back(temp);
					recruitmentSquad = Squads.at(Squads.size() - 1);	//kanske fel
					Broodwar->sendText("New recruitment squad created");
				}
				OrderUnitLocation(unit, armyGroupPoint);
			}
		}
		else {
			IdleWorkersWork(this, unit);
		}
	}
}
