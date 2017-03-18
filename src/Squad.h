#pragma once
#include <BWAPI.h>

// Handles single units in a squad. Micro, target decision (focusfire) and positioning

namespace BlackCrow {

	class BlackCrow;
	class SquadUnit;

	class Squad {
	public:

		Squad();
		~Squad();
		void onFrame();

		//std::list<BWAPI::Unit> units;
		std::set<SquadUnit*> units;

		void add(SquadUnit* unit);
		void remove(SquadUnit* unit);
		void moveAll(BWAPI::Position position, bool queue); 
		
	protected:
		BlackCrow &bc;
		Squad(BlackCrow &parent);
	};

	class ScoutSquad : public Squad {
	public:
		ScoutSquad(BlackCrow &parent);
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

	private:
		bool globalSearch;
		std::list<BWAPI::TilePosition>* scoutLocations;
	};

	class AttackSquad : public Squad {

	};

	class StallSquad : public Squad {

	};
}