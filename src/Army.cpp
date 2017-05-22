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
		auto ssIt = scoutSquads.begin();
		while (ssIt != scoutSquads.end()) {
			ScoutSquadPtr ss = *ssIt;
			if (ss->sunits.size() <= 0) {
				ssIt = scoutSquads.erase(ssIt);
			} else {
				ss->onFrame();
				ssIt++;
			}
		}

		auto asIt = attackSquads.begin();
		while (asIt != attackSquads.end()) {
			AttackSquadPtr as = *asIt;
			if (as->sunits.size() <= 0) {
				asIt = attackSquads.erase(asIt);
			} else {
				as->onFrame();
				asIt++;
			}
		}

		for (SquadUnitPtr sunit : sunits) {
			sunit->onFrame();
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

	void Army::onUnitDestroyed(BWAPI::Unit unit) {
		SquadUnitPtr sunit = findSquadUnit(unit);
		if (sunit) {
			assert(sunit->squad);
			sunit->squad->remove(sunit);
			sunits.erase(std::remove(sunits.begin(), sunits.end(), sunit), sunits.end());
		}
	}

	SquadUnitPtr Army::addToArmy(BWAPI::Unit unit) {
		sunits.emplace_back(std::make_shared<SquadUnit>(bc, unit));
		return sunits.back();
	}

	SquadUnitPtr Army::findSquadUnit(BWAPI::Unit unit) {
		for (SquadUnitPtr sunit : sunits) {
			if (sunit->unit == unit)
				return sunit;
		}
		return nullptr;
	}

	void Army::assignAutomaticSquad(SquadUnitPtr sunit) {

		if (attackSquads.size() <= 0 || attackSquads.back()->state == AttackSquad::State::ATTACK)
			attackSquads.emplace_back(std::make_shared<AttackSquad>(bc));

		AttackSquadPtr aq = attackSquads.back();
		aq->add(sunit);
		sunit->squad = aq;
	}

	void Army::startInitialScout() {
		
		BWAPI::Unit worker = bc.macro.getDroneForBuilding(bc.macro.startPosition);

		if (worker) {
			scoutSquads.emplace_back(std::make_shared<ScoutSquad>(bc));
			ScoutSquadPtr scoutSquad = scoutSquads.back();

			SquadUnitPtr sunit = addToArmy(worker);
			sunit->squad = scoutSquad;

			scoutSquad->add(sunit);
			scoutSquad->addStartLocations();
		}
	}
}