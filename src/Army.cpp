#include "Army.h"
#include <BWEM/bwem.h>
#include "BlackCrow.h"
#include "SquadUnit.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Army::Army(BlackCrow &parent) : bc(parent) {}

	void Army::onStart() {}

	void Army::onFrame() {
		for (ScoutSquadPtr ss : scoutSquads) {
			ss->onFrame();
		}

		for (AttackSquadPtr as : attackSquads) {
			as->onFrame();
		}
	}

	void Army::onUnitCreated(BWAPI::Unit unit) {
		UnitType type = unit->getType();

		if (unit->getPlayer() != Broodwar->self()
			|| type.isBuilding()
			|| type == UnitTypes::Zerg_Larva
			|| type == UnitTypes::Zerg_Drone
			|| type == UnitTypes::Zerg_Overlord)
			return;

		assignAutomaticSquad(addToArmy(unit));
	}

	SquadUnitPtr Army::addToArmy(BWAPI::Unit unit) {
		units.emplace_back(std::make_shared<SquadUnit>(unit));
		return units.back();
	}

	void Army::assignAutomaticSquad(SquadUnitPtr unitPtr) {

		if (attackSquads.size() <= 0 || attackSquads.back()->state == AttackSquad::State::ATTACK)
			attackSquads.emplace_back(std::make_shared<AttackSquad>(bc));

		AttackSquadPtr aq = attackSquads.back();
		aq->add(unitPtr);
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