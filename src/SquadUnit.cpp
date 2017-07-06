#include "SquadUnit.h"
#include "EnemyUnit.h"
#include "BlackCrow.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	SquadUnit::SquadUnit(BlackCrow& blackCrow, BWAPI::Unit unit) : bc(blackCrow), self(unit) {}

	void SquadUnit::onFrame() {
		assert(squad);

		switch (squadOverride) {
		case Override::SCOUTING:
			scout();
			break;
		case Override::MOVE_OUT_OF_WAY:
			moveOutOfWay();
			break;
		case Override::DEFILER_SNACK:
			defilerSnack();
			break;
		case Override::NONE:
			// Follow Squad Order
			switch (squad->intent) {
			case Squad::Intent::BACKDOOR:
				backdoor();
				break;
			case Squad::Intent::DEFEND:
				defend();
				break;
			case Squad::Intent::FIGHT:
				fight();
				break;
			case Squad::Intent::RUNBY:
				runby();
				break;
			case Squad::Intent::STALL:
				stall();
				break;
			}
			
			break;
		}

		/*
		if (!target->isDestroyed) {
			Unit targetBwapiUnit = Broodwar->getUnit(target->id);

			if (targetBwapiUnit->exists() && enemyBwapiUnit->isVisible()) {
				// TODO Override attack move
				if (enemyBwapiUnit && unit->getOrderTarget() != enemyBwapiUnit && !commandInQueue()) {
					unit->attack(enemyBwapiUnit, false);
					commandExecuted();
				}
			} else {
				if (!commandInQueue()) {
					attackMove(enemyUnit->position, false);
					commandExecuted();
				}
			}
		} else {
			getNewTarget();
		}
		*/
	}

	bool SquadUnit::hasTarget() {
		return target ? true : false;
	}

	bool SquadUnit::commandInQueue() {
		return nextCommandExecutesAt > Broodwar->getFrameCount() || self->getOrder() == Orders::ResetCollision;
	}

	void SquadUnit::commandExecuted() {
		nextCommandExecutesAt = Broodwar->getFrameCount() +Broodwar->getRemainingLatencyFrames();
	}

	void SquadUnit::resetSquadOverride() {
		squadOverride = Override::NONE;
	}

	void SquadUnit::squadOverrideScoutLocation(TilePosition location) {
		squadOverride = Override::SCOUTING;
		scoutLocation = location;
	}

	EnemyUnitPtr SquadUnit::getClosestTarget() {
		EnemyUnitPtr enemyUnit = bc.enemy.getClosestEnemy(self->getPosition(), [&](const EnemyUnitPtr eu) {
			Unit unit = Broodwar->getUnit(eu->id);
			return !eu->type.isFlyer()
				&& !eu->type.isInvincible()
				&& !eu->isGhost
				&& eu->type != UnitTypes::Zerg_Larva
				&& eu->type != UnitTypes::Zerg_Egg
				&& eu->type != UnitTypes::Zerg_Lurker_Egg
				&& !(!unit->isDetected() && unit->isBurrowed())
				&& !(unit->isCloaked() && !unit->isDetected())
				&& (!unit->getType().isBuilding() || Util::isFightingBuilding(eu));
			// Add under Disruption Web
		});

		if (!enemyUnit) {
			enemyUnit = bc.enemy.getClosestEnemy(self->getPosition(), [](const EnemyUnitPtr eu) {
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

		return enemyUnit;
	}

	EnemyUnitPtr SquadUnit::getClosestThreat() {
		EnemyUnitPtr enemyUnit = bc.enemy.getClosestEnemy(self->getPosition(), [](const EnemyUnitPtr eu) {
			return !eu->type.isInvincible()
				&& !eu->isGhost
				&& eu->type.canAttack()
				&& eu->type.groundWeapon() != WeaponTypes::None;
		});

		return enemyUnit;
	}

	// Commands
	void SquadUnit::move(BWAPI::Position position, bool queue = false, bool overrideCommand = false) {
		if (!commandInQueue() || overrideCommand) {
			self->move(position, queue);
			commandExecuted();
		}
	}

	void SquadUnit::attackMove(BWAPI::Position position, bool queue = false, bool overrideCommand = false) {
		if (!commandInQueue() || overrideCommand) {
			self->attack(PositionOrUnit(position), queue);
			commandExecuted();
		}
	}
	
	void SquadUnit::attack(BWAPI::Unit enemyUnit, bool queue = false, bool overrideCommand = false) {
		if (!commandInQueue() || overrideCommand) {
			self->attack(PositionOrUnit(enemyUnit), queue);
			commandExecuted();
		}
	}

	// Behaviour
	void SquadUnit::scout() {
		move(Position(scoutLocation));
	}

	void SquadUnit::moveOutOfWay() {

	}

	void SquadUnit::defilerSnack() {

	}

	void SquadUnit::backdoor() {

	}

	void SquadUnit::defend() {

	}

	void SquadUnit::fight() {

	}

	void SquadUnit::runby() {

	}

	void SquadUnit::stall() {
		EnemyUnitPtr threat = getClosestThreat();
		if (threat) {
			int keepDistance = (int)((double)self->getType().sightRange() * 0.9);
			if (Util::distance(self->getPosition(), threat->position) < keepDistance) {
				self->move(squad->squadGoalTarget->position);
			} else {
				self->move(bc.macro.startPosition);
			}
		}
	}
}