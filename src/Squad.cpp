#include "Squad.h"
#include "SquadUnit.h"
#include "BWEM/bwem.h"
#include "BlackCrow.h"
#include <numeric>

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Squad::Squad(BlackCrow &parent) : bc(parent) {}

	void Squad::onFrame() {}

	void Squad::add(const SquadUnitPtr unit) {
		sunits.push_back(unit);
	}

	void Squad::remove(const SquadUnitPtr sunit) {
		sunits.erase(std::remove(sunits.begin(), sunits.end(), sunit), sunits.end());
	}

	void Squad::moveAll(BWAPI::Position position, bool queue) {
		for (SquadUnitPtr sunit : sunits) {
			if (sunit->isIdle() || !sunit->isMoving())
				sunit->attackMove(position, queue);
		}
	}

	// ######## Scout Squad #########
	ScoutSquad::ScoutSquad(BlackCrow &parent) : Squad(parent) {}

	void ScoutSquad::onFrame() {
		Squad::onFrame();

		// Remove visible locations
		auto locIt = scoutLocations.begin();
		while (locIt != scoutLocations.end()) {
			if (Broodwar->isVisible(*locIt))
				locIt = scoutLocations.erase(locIt);
			else
				locIt++;
		}

		// Average Position of Squad
		int avgX = 0;
		int avgY = 0;
		for (SquadUnitPtr sunit : sunits) {
			avgX += sunit->unit->getPosition().x;
			avgY += sunit->unit->getPosition().y;
		}
		TilePosition avgSquadTilePosition(avgX / sunits.size(), avgY /= sunits.size());

		// Move to closest point
		if (bc.isExecutingCommandFrame())
			if (scoutLocations.size() > 0) {
				auto locIt = std::min_element(scoutLocations.begin(), scoutLocations.end(), [&](TilePosition& left, TilePosition& right) { return Util::distance(left, avgSquadTilePosition) < Util::distance(right, avgSquadTilePosition); });
				moveAll(BWAPI::Position(*locIt), false);
			}
			

		if ((scoutLocations.size() <= 0 && sunits.size() >= 0) || bc.enemy.hasKnownBuilding())
			disband();
	}

	bool ScoutSquad::isStillScouting() {
		return scoutLocations.size() > 0;
	}

	int ScoutSquad::locationsToScout() {
		return scoutLocations.size();
	}

	std::vector<BWAPI::TilePosition>& ScoutSquad::getScoutingPositions() {
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

	void ScoutSquad::addExpansions(bool addIslands) {
		auto startAreaGroupId = bc.bwem.GetArea(TilePosition(bc.macro.startPosition))->GroupId();

		for (const BWEM::Area& area : bc.bwem.Areas()) {
			for (const BWEM::Base& base : area.Bases()) {
				if (!Broodwar->isVisible(base.Location())) {
					if (addIslands) {
						addScoutPosition(base.Location());
					} else {
						if (area.GroupId() == startAreaGroupId)
							addScoutPosition(base.Location());
					}
				}
			}
		}
	}

	void ScoutSquad::setGlobalSearch(bool searchGlobally) {
		globalSearch = searchGlobally;
	}

	void ScoutSquad::disband() {
		for (SquadUnitPtr sunit : sunits) {
			if (sunit->unit->getType() == UnitTypes::Zerg_Drone) {
				bc.macro.addDrone(sunit->unit);
				bc.army.removeFromArmy(sunit);
			} else {
				bc.army.assignAutomaticAttackSquad(sunit);
			}
		}
		sunits.clear();
	}

	// ######## Attack Squad #########
	AttackSquad::AttackSquad(BlackCrow &parent) : Squad(parent) {}

	void AttackSquad::onFrame() {
		Squad::onFrame();

		static int attackSize = 10;
		// Decide
		if ((int)sunits.size() >= attackSize) {
			state = State::ATTACK;
			attackSize += 3;
		}

		if (state == State::MOVE) {
			for (SquadUnitPtr sunit : sunits) {
				if (sunit->unit->isUnderAttack()) {
					state = State::ATTACK;
					attackSize += 1;
					break;
				}
			}
		}

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
				EnemyUnit* enemyUnit = bc.enemy.getClosestEnemy(sunit->unit->getPosition(), [&](const EnemyUnit& eu) { 
					Unit unit = Broodwar->getUnit(eu.id);
					return !eu.type.isFlyer()
						&& !eu.type.isInvincible()
						&& !eu.isGhost
						&& eu.type != UnitTypes::Zerg_Larva
						&& eu.type != UnitTypes::Zerg_Egg
						&& eu.type != UnitTypes::Zerg_Lurker_Egg
						&& !(!unit->isDetected() && unit->isBurrowed())
						&& (!unit->getType().isBuilding() || isFightingBuilding(eu));
						// Add under Disruption Web
				});

				if (!enemyUnit) {
					enemyUnit = bc.enemy.getClosestEnemy(sunit->unit->getPosition(), [](const EnemyUnit& eu) {
						Unit unit = Broodwar->getUnit(eu.id);
						return !eu.type.isFlyer()
							&& !eu.type.isInvincible()
							&& !eu.isGhost
							&& eu.type != UnitTypes::Zerg_Larva
							&& eu.type != UnitTypes::Zerg_Egg
							&& eu.type != UnitTypes::Zerg_Lurker_Egg
							&& !(!unit->isDetected() && unit->isBurrowed());
						// Add under Disruption Web
					});
				}

				if (enemyUnit)
					sunit->setAttackTarget(enemyUnit->id);
			}

			if (!bc.enemy.hasKnownBuilding()) {
				// Assign all to scouts
				for (SquadUnitPtr sunit : sunits)
					bc.army.assignAutomaticScoutSquad(sunit);
				sunits.clear();
			}

			break;
		}
	}

	bool AttackSquad::isFightingBuilding(const EnemyUnit& eu) {
		return eu.type == UnitTypes::Zerg_Sunken_Colony
			|| eu.type == UnitTypes::Zerg_Spore_Colony
			|| eu.type == UnitTypes::Protoss_Photon_Cannon
			|| eu.type == UnitTypes::Terran_Bunker;
	}
}