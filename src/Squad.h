#pragma once
#include <BWAPI.h>
#include "SquadUnit.h"
#include <BWEM/bwem.h> 

// Handles single units in a squad. Micro, target decision (focusfire) and positioning

class Squad {

public:
	void init();
	void onFrame();

	//std::list<BWAPI::Unit> units;
	std::set<SquadUnit*> units;

	void add(SquadUnit* unit);
	void remove(SquadUnit* unit);
	void moveAll(BWAPI::Position position, bool queue);
	
};

class ScoutSquad : public Squad {
	bool globalSearch;
	std::list<BWAPI::TilePosition>* scoutLocations;
public:

	ScoutSquad();
	~ScoutSquad();
	void init();
	void onFrame();
	bool isStillScouting();
	int locationsToScout();
	std::list<BWAPI::TilePosition>* getScoutingPositions();
	void addScoutPosition(BWAPI::TilePosition position);
	void addStartLocations();
	void addExpansions();
	void setGlobalSearch(bool searchGlobally);
};

class AttackSquad : public Squad {

};

class StallSquad : public Squad {

};