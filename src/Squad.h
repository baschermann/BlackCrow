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

		enum class Intent {
			STALL,
			FIGHT,
			BACKDOOR,
			RUNBY,
			DEFEND
		};

		struct ScoutLocation {
			BWAPI::TilePosition location;
			std::vector<SquadUnitPtr> assigned;
			std::function<bool(void)> cancelCondition;

			ScoutLocation(BWAPI::TilePosition location) : location(location) {}

			template <class UnaryPredicate>
			ScoutLocation(BWAPI::TilePosition location, UnaryPredicate cancelCondition) : location(location), cancelCondition(cancelCondition) {}

		};

		Squad(BlackCrow &parent);
		void onFrame();

		Intent intent = Intent::STALL;
		std::vector<SquadUnitPtr> sunits;
		std::vector<ScoutLocation> scoutLocations;

		void add(const SquadUnitPtr& sunit);
		void remove(const SquadUnitPtr& sunit);
		int size();
		

		// Scouting
		bool isScouting();
		int locationsToScoutLeft();
		void addScoutLocation(const BWAPI::TilePosition& position);
		void addScoutLocationsStartingPoints();
		void addScoutLocationsExpansions(bool addIslands);

		// Fighting
		EnemyUnitPtr squadGoalTarget = nullptr;

	private:

		BlackCrow &bc;
		void adjustTarget();
		void removeScoutedLocations();
		void removeAssignedScoutUnits(ScoutLocation& scoutLocation);
		void assignSunitsToScoutLocations();
	};
}