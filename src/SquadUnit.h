#pragma once
#include <BWAPI.h>
#include "Common.h"

namespace BlackCrow {

	class SquadUnit {
	public:
		SquadUnit(BWAPI::Unit unit);

		// Variables
		BWAPI::Unit unit;
		SquadPtr squad = nullptr;
		int lastCommandSent = 0;
		int nextCommandExecutesAt = 0;

		// Functions
		bool isIdle();
		void move(BWAPI::Position position, bool queue);
		bool isMoving();
		void attack(BWAPI::PositionOrUnit pou, bool queue);
		bool SquadUnit::commandInQueue();
		void SquadUnit::commandExecuted();
	};
}