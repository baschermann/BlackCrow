#pragma once
#include <memory>
#include <BWAPI.h>
#include "Common.h"

namespace BlackCrow {

	class BlackCrow;
	class ScoutSquad;
	class AttackSquad;

	class Army {
	public:
		Army(BlackCrow &blackrow);

		void onStart();
		void onFrame();

		// Variables
		std::vector<SquadUnitPtr> sunits;
		std::vector<ScoutSquadPtr> scoutSquads;
		std::vector<AttackSquadPtr> attackSquads;

		// Functions
		void onUnitCreated(BWAPI::Unit unit);
		void onUnitDestroyed(BWAPI::Unit unit);
		SquadUnitPtr addToArmy(BWAPI::Unit unit);
		SquadUnitPtr findSquadUnit(BWAPI::Unit unit);
		void assignAutomaticSquad(SquadUnitPtr);
		void startInitialScout();

	private:
		BlackCrow &bc;
	};

}