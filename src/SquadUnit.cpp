#include "SquadUnit.h"
#include "EnemyUnit.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	void SquadUnit::onFrame() {
		if (attackTarget) {
			Unit enemyUnit = Broodwar->getUnit(attackTarget->id);

			// TODO Override attack move
			if (attackTarget && unit->getOrderTarget() != enemyUnit && !commandInQueue()) { // TODO override attack move
				unit->attack(enemyUnit, false);
				commandExecuted();

				if (unit->isSelected())
					Broodwar->sendText("Attack Enemy %i", enemyUnit->getID());
			}
		}
	}

	SquadUnit::SquadUnit(BWAPI::Unit unit) {
		this->unit = unit;
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

	void SquadUnit::setAttackTarget(EnemyUnit& enemy) {
		attackTarget = &enemy;
	}

	bool SquadUnit::hasTarget() {
		if (!attackTarget)
			return true;

		if (Broodwar->getUnit(attackTarget->id)->exists()) {
			return true;
		}

		return false;
	}

	bool SquadUnit::commandInQueue() {
		return nextCommandExecutesAt > Broodwar->getFrameCount() || unit->getOrder() == Orders::ResetCollision;
	}

	void SquadUnit::commandExecuted() {
		nextCommandExecutesAt = Broodwar->getFrameCount() +Broodwar->getRemainingLatencyFrames();
	}
}