#pragma once
#include <memory>
#include <BWAPI.h>
#include "Common.h"

namespace BlackCrow {

	class BlackCrow;
	class ScoutSquad;

	class Army {
	public:
		Army(BlackCrow &blackrow);

		void onStart();
		void onFrame();

		// Variables
		std::vector<SquadUnitPtr> units;
		std::vector<ScoutSquadPtr> scoutSquads;

		// Functions
		SquadUnitPtr addToArmy(BWAPI::Unit unit);
		void assignAutomaticSquad(SquadUnitPtr);
		void startInitialScout();

	private:
		BlackCrow &bc;
	};

}