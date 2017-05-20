#pragma once
#include <BWAPI.h>
#include "Common.h"

namespace BlackCrow {

	class EnemyUnit;

	class SquadUnit {
	public:
		SquadUnit(BWAPI::Unit unit);

		// Variables
		BWAPI::Unit unit = nullptr;
		EnemyUnit* attackTarget = nullptr;
		SquadPtr squad = nullptr;
		int lastCommandSent = 0;
		int nextCommandExecutesAt = 0;

		// Functions
		void onFrame();
		bool isIdle();
		void move(BWAPI::Position position, bool queue);
		bool isMoving();
		void attackMove(BWAPI::Position position, bool queue);
		void setAttackTarget(EnemyUnit& enemy);
		bool hasTarget();
		bool commandInQueue();
		void commandExecuted();
	};
}