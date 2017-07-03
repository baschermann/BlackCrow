#pragma once
#include <BWAPI.h>
#include "Common.h"

namespace BlackCrow {

	class BlackCrow;
	class EnemyUnit;

	class SquadUnit {
	public:
		SquadUnit(BlackCrow& bc, BWAPI::Unit unit);
		void onFrame();

		enum class OVERRIDE {
			SCOUTING,
			MOVE_OUT_OF_WAY,
			DEFILER_SNACK
		};

		// Squad
		bool squadOverride = false;
		BWAPI::TilePosition scoutLocation = TilePositions::None;
		void resetSquadOverride();
		void squadOverrideScoutLocation(BWAPI::TilePosition location);

		// Variables
		BWAPI::Unit unit = nullptr;
		int attackTargetId = -13337;
		SquadPtr squad = nullptr;
		int lastCommandSent = 0;
		int nextCommandExecutesAt = 0;

		// Functions
		bool isIdle();
		void move(BWAPI::Position position, bool queue);
		bool isMoving();
		void attackMove(BWAPI::Position position, bool queue);
		void setAttackTarget(int id);
		bool hasTarget();
		bool commandInQueue();
		void commandExecuted();

	private:
		BlackCrow &bc;
	};
}