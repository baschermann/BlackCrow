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
		for (ScoutSquadPtr ss : scoutSquads) {
			ss->onFrame();
		}
	}

	SquadUnitPtr Army::addToArmy(BWAPI::Unit unit) {
		units.emplace_back(std::make_shared<SquadUnit>(unit));
		return units.back();
	}

	void Army::assignAutomaticSquad(SquadUnitPtr) {
	}

	void Army::startInitialScout() {
		
		BWAPI::Unit worker = bc.macro.getDroneForBuilding(bc.macro.startPosition);

		if (worker) {
			scoutSquads.emplace_back(std::make_shared<ScoutSquad>(bc));
			ScoutSquadPtr scoutSquad = scoutSquads.back();

			scoutSquad->add(addToArmy(worker));
			scoutSquad->addStartLocations();
		}
	}
}