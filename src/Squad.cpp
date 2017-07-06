#include "Squad.h"
#include "SquadUnit.h"
#include "BWEM/bwem.h"
#include "BlackCrow.h"
#include <numeric>

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Squad::Squad(BlackCrow &parent) : bc(parent) {}

	void Squad::onFrame() {
		// Looking for a squad goal myself. The army should be assigning targets
		adjustTarget();

		// Remove scouted Locations and free Units
		removeScoutedLocations();

		// For all scouting points get the closest unit
		assignSunitsToScoutLocations();

		// Call onFrame for all of the squad units
		for (auto& sunit : sunits)
			sunit->onFrame();
	}

	void Squad::add(const SquadUnitPtr& unit) {
		sunits.push_back(unit);
	}

	void Squad::remove(const SquadUnitPtr& sunit) {
		sunits.erase(std::remove(sunits.begin(), sunits.end(), sunit), sunits.end());
	}

	int Squad::size() {
		return sunits.size();
	}

	// Scouting
	bool Squad::isScouting() {
		return scoutLocations.size() > 0;
	}

	int Squad::locationsToScoutLeft() {
		return scoutLocations.size();
	}

	void Squad::addScoutLocation(const BWAPI::TilePosition& position) {
		scoutLocations.push_back(ScoutLocation(position));
	}

	void Squad::addScoutLocationsStartingPoints() {
		for (BWAPI::TilePosition tilePosition : bc.bwem.StartingLocations()) {
			if (!Broodwar->isVisible(tilePosition)) {
				tilePosition.x += 1;
				tilePosition.y += 1;
				addScoutLocation(tilePosition);
			}
		}
	}

	void Squad::addScoutLocationsExpansions(bool addIslands) {
		for (const Base& base : bc.macro.bases)
			if (!Broodwar->isVisible(base.bwemBase.Location()))
				if (addIslands)
					addScoutLocation(base.bwemBase.Location());
				else
					if (!base.isIsland)
						addScoutLocation(base.bwemBase.Location());
	}


	// ### Private ###
	void Squad::adjustTarget() {
		EnemyUnitPtr enemyUnit = bc.enemy.getClosestEnemy(bc.macro.startPosition, [](const EnemyUnitPtr eu) {
			Unit unit = Broodwar->getUnit(eu->id);
			return !eu->type.isFlyer()
				&& !eu->type.isInvincible()
				&& !eu->isGhost
				&& eu->type != UnitTypes::Zerg_Larva
				&& eu->type != UnitTypes::Zerg_Egg
				&& eu->type != UnitTypes::Zerg_Lurker_Egg
				&& !(!unit->isDetected() && unit->isBurrowed())
				&& !(unit->isCloaked() && !unit->isDetected())
				&& !(!unit->isDetected() && unit->isBurrowed());
			// Add under Disruption Web
		});

		if (enemyUnit)
			squadGoalTarget = enemyUnit;
		else
			if (scoutLocations.size() <= 0)
				addScoutLocationsExpansions(false);
	}

	void Squad::removeScoutedLocations() {
		auto scoutLocationIt = scoutLocations.begin();
		while (scoutLocationIt != scoutLocations.end()) {
			ScoutLocation& scoutLocation = *scoutLocationIt;

			// If they have no cancel condition, remove if their scouting locations are visible
			if (!scoutLocation.cancelCondition) {
				if (Broodwar->isVisible(scoutLocation.location)) {
					removeAssignedScoutUnits(scoutLocation);
					scoutLocationIt = scoutLocations.erase(scoutLocationIt);
				} else
					scoutLocationIt++;
			} else {
				// Check cancel condition
				if (scoutLocation.cancelCondition()) {
					removeAssignedScoutUnits(scoutLocation);
					scoutLocationIt = scoutLocations.erase(scoutLocationIt);
				} else
					scoutLocationIt++;
			}
		}
	}

	void Squad::removeAssignedScoutUnits(ScoutLocation& scoutLocation) {
		for (auto& sunit : scoutLocation.assigned)
			if (sunit->squadOverride == SquadUnit::Override::SCOUTING)
				sunit->resetSquadOverride();
	}

	void Squad::assignSunitsToScoutLocations() {
		for (auto& scoutLocation : scoutLocations) {
			if (scoutLocation.assigned.size() <= 0) {
				SquadUnitPtr closestUnit = nullptr;
				double closestDistance = std::numeric_limits<double>().max();

				for (auto& sunit : sunits) {
					if (sunit->squadOverride == SquadUnit::Override::NONE) {
						double distance = Util::distance(Position(scoutLocation.location), sunit->self->getPosition());
						if (distance < closestDistance) {
							closestUnit = sunit;
							closestDistance = distance;
						}
					}
				}

				if (closestUnit) {
					scoutLocation.assigned.push_back(closestUnit);
					closestUnit->squadOverrideScoutLocation(scoutLocation.location);
				}
			}
		}



		// Put unused drones back to mining
		auto& droneIt = std::find_if(sunits.begin(), sunits.end(), [](SquadUnitPtr& sunit) { 
			return sunit->squadOverride != SquadUnit::Override::SCOUTING
				&& sunit->self->getType() == UnitTypes::Zerg_Drone; 
		});
		if (droneIt != sunits.end()) {
			bc.macro.addDrone((*droneIt)->self);
			bc.army.releaseFromArmy(*droneIt);
		}

	}




		/*
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


	// ######## Attack Squad #########
	AttackSquad::AttackSquad(BlackCrow &parent) : Squad(parent) {}

	void AttackSquad::onFrame() {
		Squad::onFrame();

		static int attackSize = 14;
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

					const BWEM::Area* area = bc.bwem.GetArea(TilePosition(bc.macro.startPosition));
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
				EnemyUnitPtr enemyUnit = bc.enemy.getClosestEnemy(sunit->unit->getPosition(), [&](const EnemyUnitPtr eu) { 
					Unit unit = Broodwar->getUnit(eu->id);
					return !eu->type.isFlyer()
						&& !eu->type.isInvincible()
						&& !eu->isGhost
						&& eu->type != UnitTypes::Zerg_Larva
						&& eu->type != UnitTypes::Zerg_Egg
						&& eu->type != UnitTypes::Zerg_Lurker_Egg
						&& !(!unit->isDetected() && unit->isBurrowed())
						&& !(unit->isCloaked() && !unit->isDetected())
						&& (!unit->getType().isBuilding() || isFightingBuilding(eu));
						// Add under Disruption Web
				});

				if (!enemyUnit) {
					enemyUnit = bc.enemy.getClosestEnemy(sunit->unit->getPosition(), [](const EnemyUnitPtr eu) {
						Unit unit = Broodwar->getUnit(eu->id);
						return !eu->type.isFlyer()
							&& !eu->type.isInvincible()
							&& !eu->isGhost
							&& eu->type != UnitTypes::Zerg_Larva
							&& eu->type != UnitTypes::Zerg_Egg
							&& eu->type != UnitTypes::Zerg_Lurker_Egg
							&& !(!unit->isDetected() && unit->isBurrowed())
							&& !(unit->isCloaked() && !unit->isDetected())
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
	*/
}