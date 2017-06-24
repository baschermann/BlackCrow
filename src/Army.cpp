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

		assignAutomaticAttackSquad(addToArmy(unit));
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

	void Army::removeFromArmy(SquadUnitPtr sunit) {
		assert(sunit->squad);
		sunits.erase(std::remove(sunits.begin(), sunits.end(), sunit), sunits.end());
	}

	SquadUnitPtr Army::findSquadUnit(BWAPI::Unit unit) {
		for (SquadUnitPtr sunit : sunits) {
			if (sunit->unit == unit)
				return sunit;
		}
		return nullptr;
	}

	void Army::assignAutomaticAttackSquad(SquadUnitPtr sunit) {

		if (attackSquads.size() <= 0 || attackSquads.back()->state == AttackSquad::State::ATTACK)
			createAttackSquad();

		AttackSquadPtr aq = attackSquads.back();
		aq->add(sunit);
		sunit->squad = aq;
	}

	void Army::assignAutomaticScoutSquad(SquadUnitPtr sunit) {

		if (scoutSquads.size() <= 0) {
			ScoutSquadPtr ss = createScoutSquad();
			ss->addExpansions(false);
		}

		ScoutSquadPtr ss = scoutSquads.back();
		ss->add(sunit);
		sunit->squad = ss;
	}

	void Army::startInitialScout() {
		
		BWAPI::Unit worker = bc.macro.getDroneForBuilding(bc.macro.startPosition);

		if (worker) {
			ScoutSquadPtr scoutSquad = createScoutSquad();

			SquadUnitPtr sunit = addToArmy(worker);
			sunit->squad = scoutSquad;

			scoutSquad->add(sunit);
			scoutSquad->addStartLocations();
		}
	}

	AttackSquadPtr Army::createAttackSquad() {
		attackSquads.emplace_back(std::make_shared<AttackSquad>(bc));
		return attackSquads.back();
	}

	ScoutSquadPtr Army::createScoutSquad() {
		scoutSquads.emplace_back(std::make_shared<ScoutSquad>(bc));
		return scoutSquads.back();
	}

	void Army::workerUnderAttack(WorkerPtr workerUnderAttack, Base& base) {
		std::vector<WorkerPtr> closeWorkers;

		for (WorkerPtr worker : base.workers) {
			if (Util::distance(workerUnderAttack->unit->getPosition(), worker->unit->getPosition()) < 48)
				closeWorkers.push_back(worker);
		}

		EnemyUnitPtr eu = bc.enemy.getClosestEnemy(workerUnderAttack->unit->getPosition(), [](EnemyUnitPtr eu) { return true; });
		if (eu) {
			for (WorkerPtr worker : closeWorkers) {
				if (!worker->unit->isAttacking()) {
					worker->stopMining();
					worker->unit->attack(Broodwar->getUnit(eu->id));
				}
				worker->resetAfterAttackFrameCounter = 40;
			}
		}
	}
}