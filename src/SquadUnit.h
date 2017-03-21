#pragma once
#include <BWAPI.h>

namespace BlackCrow {

	class SquadUnit {
	public:
		SquadUnit(BWAPI::Unit unit);

		BWAPI::Unit unit;

		void move(BWAPI::Position position, bool queue);
	};
}