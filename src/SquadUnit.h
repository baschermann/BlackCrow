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

		enum class Override {
			NONE,
			SCOUTING,
			MOVE_OUT_OF_WAY,
			DEFILER_SNACK
		};

		// Squad
		Override squadOverride = Override::NONE;
		BWAPI::TilePosition scoutLocation = BWAPI::TilePositions::None;
		void resetSquadOverride();
		void squadOverrideScoutLocation(BWAPI::TilePosition location);

		// Variables
		BWAPI::Unit self = nullptr;
		EnemyUnitPtr target = nullptr;
		SquadPtr squad = nullptr;
		int lastCommandSent = 0;
		int nextCommandExecutesAt = 0;

		// Functions
		bool hasTarget();
		bool commandInQueue();
		void commandExecuted();

	private:
		BlackCrow &bc;

		EnemyUnitPtr getClosestTarget();
		EnemyUnitPtr getClosestThreat();

		void move(BWAPI::Position position, bool queue, bool overrideCommand);
		void attackMove(BWAPI::Position position, bool queue, bool overrideCommand);
		void attack(BWAPI::Unit enemyUnit, bool queue, bool overrideCommand);

		void scout();
		void moveOutOfWay();
		void defilerSnack();
		void backdoor();
		void defend();
		void fight();
		void runby();
		void stall();
			
	};
}