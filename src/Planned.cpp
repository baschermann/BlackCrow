#include "Planned.h"
#include "BlackCrow.h"

namespace BlackCrow {
	Planned::Planned(BlackCrow &parent) : bc(parent), status(Status::ACTIVE) {}


	PlannedUnitt::PlannedUnitt(BlackCrow &parent, BWAPI::UnitType type) : Planned(parent) {}


	int PlannedUnitt::getMineralCost() {
		return 0;
	}

	int PlannedUnitt::getGasCost() {
		return 0;
	}
}