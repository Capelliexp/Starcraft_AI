#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include <utility>
#include <cmath>
#include <vector>

extern bool analyzed;
extern bool analysis_just_finished;
extern BWTA::Region* home;
extern BWTA::Region* enemy_base;
DWORD WINAPI AnalyzeThread();

using namespace BWAPI;
using namespace BWTA;

struct subTask {
	short task, step, base;
	UnitType construct;
	short requiredUnits, completedUnits, inProgressUnits;
};

struct Squad {
	std::vector<BWAPI::Unit> members;
	BWAPI::Order command;
	BWAPI::Position position;
};

class MyBot : public BWAPI::AIModule
{
public:
	//Methods inherited from BWAPI:AIModule
	virtual void onStart();
	virtual void onEnd(bool isWinner);
	virtual void onFrame();
	virtual void onSendText(std::string text);
	virtual void onReceiveText(BWAPI::Player player, std::string text);
	virtual void onPlayerLeft(BWAPI::Player player);
	virtual void onNukeDetect(BWAPI::Position target);
	virtual void onUnitDiscover(BWAPI::Unit unit);
	virtual void onUnitEvade(BWAPI::Unit unit);
	virtual void onUnitShow(BWAPI::Unit unit);
	virtual void onUnitHide(BWAPI::Unit unit);
	virtual void onUnitCreate(BWAPI::Unit unit);
	virtual void onUnitDestroy(BWAPI::Unit unit);
	virtual void onUnitMorph(BWAPI::Unit unit);
	virtual void onUnitRenegade(BWAPI::Unit unit);
	virtual void onSaveGame(std::string gameName);
	virtual void onUnitComplete(BWAPI::Unit unit);

	void drawStats();
	void drawTerrainData();
	void showPlayers();
	void showForces();
	Position findGuardPoint();

	//--------------------------------------------
	//My superior intellect applied to a state of the art artificial intelligence:
	short frameCount10;
	short frameCount100;
	short frameCount1000;
	short sanityCount;

	subTask* task;
	subTask currentSubTask;
	int currentSubTaskNr;

	std::vector<BWTA::BaseLocation*> CloseBaseLocations;
	std::vector<BWTA::BaseLocation*> EnemyBases;
	std::vector<Position> PotentialEnemyBasePositions;
	std::vector<Position> AllBasePositions;

	std::vector<Squad*> Squads;
	Squad* recruitmentSquad;

	Position armyGroupPoint;

	
};
