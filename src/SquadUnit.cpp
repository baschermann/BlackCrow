#include "SquadUnit.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	SquadUnit::SquadUnit(BWAPI::Unit unit) {
		this->unit = unit;
	}


	SquadUnit::~SquadUnit() {
	}

	void SquadUnit::move(BWAPI::Position position, bool queue) {
		unit->move(position, queue);
	}
}