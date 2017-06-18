#include "SquadUnit.h"
#include "EnemyUnit.h"
#include "BlackCrow.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	SquadUnit::SquadUnit(BlackCrow& blackCrow, BWAPI::Unit unit) : bc(blackCrow), unit(unit) {}

	void SquadUnit::onFrame() {
		EnemyUnit* enemyUnit = bc.enemy.getEnemy(attackTargetId);

		if (enemyUnit) {
			Unit enemyBwapiUnit = Broodwar->getUnit(enemyUnit->id);

			if (enemyBwapiUnit->exists() && enemyBwapiUnit->isVisible()) {
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
		}
	}

	bool SquadUnit::isIdle() {
		return unit->isIdle() && !commandInQueue();
	}

	void SquadUnit::move(BWAPI::Position position, bool queue) {
		if (!commandInQueue()) {
			unit->move(position, queue);
			commandExecuted();
		}
	}

	bool SquadUnit::isMoving() {
		return unit->getOrder() == Orders::Move;
	}

	void SquadUnit::attackMove(BWAPI::Position position, bool queue) {
		if (!commandInQueue()) {
			unit->attack(position, queue);
			commandExecuted();
		}
	}

	void SquadUnit::setAttackTarget(int id) {
		attackTargetId = id;
	}

	bool SquadUnit::hasTarget() {
		EnemyUnit* enemyUnit = bc.enemy.getEnemy(attackTargetId);

		if (enemyUnit)
			return true;

		return false;
	}

	bool SquadUnit::commandInQueue() {
		return nextCommandExecutesAt > Broodwar->getFrameCount() || unit->getOrder() == Orders::ResetCollision;
	}

	void SquadUnit::commandExecuted() {
		nextCommandExecutesAt = Broodwar->getFrameCount() +Broodwar->getRemainingLatencyFrames();
	}
}