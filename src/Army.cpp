#include "Army.h"
#include <BWEM/bwem.h>
#include "BlackCrow.h"
#include "SquadUnit.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Army::Army(BlackCrow &parent) : bc(parent) {}

	void Army::onStart() {

	}

	void Army::onFrame() {
		for (ScoutSquad& ss : scoutSquads) {
			ss.onFrame();
		}
	}

	SquadUnitPtr Army::addToArmy(BWAPI::Unit unit) {
		units.emplace_back(std::make_shared<SquadUnit>(unit));
		return units.back();
	}

	SquadPtr Army::assignAutomaticSquad(SquadUnitPtr) {
		return nullptr;
	}

	void Army::startInitialScout() {
		
		BWAPI::Unit worker = bc.macro.getDroneForBuilding(bc.macro.startPosition);

		if (worker) {
			scoutSquads.emplace_back(bc);
			ScoutSquad scoutSquad = scoutSquads.back();

			scoutSquad.add(addToArmy(worker));
			scoutSquad.addStartLocations();
		}
	}
}