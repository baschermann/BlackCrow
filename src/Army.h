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
		std::vector<ScoutSquadPtr> scoutSquads;
		std::vector<AttackSquadPtr> attackSquads;

		// Functions
		void onUnitCreated(BWAPI::Unit unit);
		void onUnitDestroyed(BWAPI::Unit unit);
		SquadUnitPtr addToArmy(BWAPI::Unit unit);
		void removeFromArmy(SquadUnitPtr sunit);
		SquadUnitPtr findSquadUnit(BWAPI::Unit unit);
		AttackSquadPtr createAttackSquad();
		ScoutSquadPtr createScoutSquad();
		void assignAutomaticAttackSquad(SquadUnitPtr sunit);
		void assignAutomaticScoutSquad(SquadUnitPtr sunit);
		void startInitialScout();
		void workerUnderAttack(WorkerPtr worker, Base& base);

	private:
		BlackCrow &bc;
	};

}