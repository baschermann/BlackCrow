#pragma once
#include <memory>
#include <BWAPI.h>
#include "Common.h"

namespace BlackCrow {

	class BlackCrow;
	class ScoutSquad;
	class AttackSquad;
	class Base;

	class Army {
	public:
		Army(BlackCrow &blackrow);

		void onStart();
		void onFrame();

		// Variables
		std::vector<SquadUnitPtr> sunits;
		std::vector<SquadPtr> squads;

		// Functions
		void onUnitCreated(BWAPI::Unit unit);
		void onUnitDestroyed(BWAPI::Unit unit);
		SquadUnitPtr& addToArmy(BWAPI::Unit unit);
		void releaseFromArmy(SquadUnitPtr& sunit);
		SquadUnitPtr findSquadUnit(BWAPI::Unit unit);
		void assignToSquad(SquadUnitPtr& sunit);
		void startInitialScout();
		void workerUnderAttack(WorkerPtr& worker, Base& base);

	private:
		BlackCrow &bc;
		void runSquadsAndUnits();
		SquadPtr& createSquad();
		SquadPtr& getOrCreateSquadIfNoneExist();
	};

}