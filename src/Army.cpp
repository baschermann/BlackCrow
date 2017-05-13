#include "Army.h"
#include <BWEM/bwem.h>
#include "BlackCrow.h"
#include "SquadUnit.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Army::Army(BlackCrow &parent) : bc(parent) {}

	void Army::addUnit(BWAPI::Unit unit) {
		units.emplace_back(std::make_shared<SquadUnit>(unit));
	}

	SquadPtr Army::assignToSquad(SquadUnitPtr) {
		return nullptr;
	}
}