#pragma once
#include <BWAPI.h>
#include <memory>
#include "SquadUnit.h"
#include "Common.h"

// Notes:
//
// A "stall" or "retreat" order does not order all units in the squad to retreat
// but rather gives the suqad units the command to retreat if in enemy range and necessary
// if they have no enemy nearby they will collect at a position

namespace BlackCrow {

	class BlackCrow;
	class SquadUnit;

	class Squad {
	public:

		enum class INTENT {
			STALL,
			FIGHT,
			BACKDOOR,
			RUNBY,
			DEFEND
		};

		Squad(BlackCrow &parent);
		void onFrame();

		void add(const SquadUnitPtr& sunit);
		void remove(const SquadUnitPtr& sunit);
		std::vector<SquadUnitPtr>& getSquadUnits();
		int size();

		// Scouting
		bool isScouting();
		int locationsToScoutLeft();
		std::vector<BWAPI::TilePosition>& getScoutingLocations();
		void addScoutLocation(const BWAPI::TilePosition& position);
		void addScoutLocationsStartingPoints();
		void addScoutLocationsExpansions(bool addIslands);

		// Fighting
		EnemyUnitPtr squadGoalTarget = nullptr;
		bool isFightingBuilding(const EnemyUnitPtr eu);

	private:
		struct ScoutLocation {
			BWAPI::TilePosition location;
			std::vector<SquadUnitPtr> assigned;

			ScoutLocation(BWAPI::TilePosition location) : location(location) {}
		};

		BlackCrow &bc;
		std::vector<SquadUnitPtr> sunits;
		std::vector<ScoutLocation> scoutLocations;
		void adjustTarget();
	};
}