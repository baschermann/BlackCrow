#include "Squad.h"
#include "SquadUnit.h"
#include "BWEM/bwem.h"
#include "BlackCrow.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Squad::Squad(BlackCrow &parent) : bc(parent) {}

	void Squad::onFrame() {}

	void Squad::add(SquadUnitPtr unit) {
		sunits.push_back(unit);
	}

	void Squad::remove(SquadUnitPtr unit) {
		sunits.erase(std::remove(sunits.begin(), sunits.end(), unit), sunits.end());
	}

	void Squad::moveAll(BWAPI::Position position, bool queue) {
		for (SquadUnitPtr sunit : sunits) {
			if (sunit->isIdle() || !sunit->isMoving())
				sunit->move(position, queue);
		}
	}

	// ######## Scout Squad #########
	ScoutSquad::ScoutSquad(BlackCrow &parent) : Squad(parent) {}

	void ScoutSquad::onFrame() {
		Squad::onFrame();

		while (scoutLocations.size() > 0 && Broodwar->isVisible(scoutLocations.back()))
			scoutLocations.pop_back();

		if (bc.isExecutingCommandFrame())
			if (scoutLocations.size() > 0)
				moveAll(BWAPI::Position(scoutLocations.back()), false); // Kruecke: This will spam hard! - TODO Not good then!
	}

	bool ScoutSquad::isStillScouting() {
		return scoutLocations.size() > 0;
	}

	int ScoutSquad::locationsToScout() {
		return scoutLocations.size();
	}

	std::vector<BWAPI::TilePosition> ScoutSquad::getScoutingPositions() {
		return scoutLocations;
	}

	void ScoutSquad::addScoutPosition(BWAPI::TilePosition tilePosition) {
		scoutLocations.push_back(tilePosition);
	}

	void ScoutSquad::addStartLocations() {
		for (BWAPI::TilePosition tilePosition : bc.bwem.StartingLocations()) {
			if (!Broodwar->isVisible(tilePosition)) {
				tilePosition.x += 1;
				tilePosition.y += 1;
				addScoutPosition(tilePosition);
			}
		}
	}

	void ScoutSquad::addExpansions() {
		for (BWEM::Area area : bc.bwem.Areas()) {
			for (BWEM::Base base : area.Bases()) {
				if (!Broodwar->isVisible(base.Location())) {
					addScoutPosition(base.Location());
				}
			}
		}
	}

	void ScoutSquad::setGlobalSearch(bool searchGlobally) {
		globalSearch = searchGlobally;
	}

	// ######## Attack Squad #########
	AttackSquad::AttackSquad(BlackCrow &parent) : Squad(parent) {}

	void AttackSquad::onFrame() {
		Squad::onFrame();

		// Decide
		if (sunits.size() >= 20)
			state = State::ATTACK;

		// Action
		switch (state) {
		case State::MOVE:
			for (SquadUnitPtr sunit : sunits) {
				if (sunit->isIdle()) {

					const BWEM::Area* area = bc.bwem.GetArea(sunit->unit->getTilePosition());
					if (area) {
						for (const BWEM::ChokePoint* cp : area->ChokePoints()) {
							if (!cp->Blocked()) {
								sunit->attackMove(Position(cp->Center()), false);
							}
						}
					}
				}
			}
			break;

		case State::ATTACK:
			for (SquadUnitPtr sunit : sunits) {
				EnemyUnit* enemyUnit = bc.enemy.getClosestEnemy(sunit->unit->getPosition(), [](const EnemyUnit& eu) { 
					return !eu.type.isFlyer()
						&& !eu.isGhost
						&& eu.type != UnitTypes::Zerg_Larva
						&& eu.type != UnitTypes::Zerg_Egg;
				});

				if (enemyUnit)
					sunit->setAttackTarget(enemyUnit->id);
				// else we should scout for one!
			}
			break;
		}
	}
}