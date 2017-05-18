#include "SquadUnit.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

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

	void SquadUnit::attack(PositionOrUnit positionOrUnit, bool queue) {
		if (!commandInQueue()) {
			unit->attack(positionOrUnit, queue);
			commandExecuted();
		}
	}

	bool SquadUnit::commandInQueue() {
		return nextCommandExecutesAt > Broodwar->getFrameCount() || unit->getOrder() == Orders::ResetCollision;
	}

	void SquadUnit::commandExecuted() {
		nextCommandExecutesAt = Broodwar->getFrameCount() +Broodwar->getRemainingLatencyFrames();
	}
}