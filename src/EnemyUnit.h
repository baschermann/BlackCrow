#pragma once
#include <BWAPI.h>
#include "Common.h"

namespace BlackCrow {

	class EnemyUnit {
	public:
		EnemyUnit();

		int id;
		AreaPtr area;
		BWAPI::UnitType type;
		BWAPI::Position position;
		BWAPI::TilePosition tilePosition;
		int lastSeen;
		bool isVisible;
		bool isGhost = false;
		bool isDestroyed = false;
	};

	bool operator==(const EnemyUnit& left, const EnemyUnit& right);

}