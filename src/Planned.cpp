#include "Planned.h"
#include "BlackCrow.h"

namespace BlackCrow {
	Planned::Planned(BlackCrow &parent) : bc(parent), status(Status::ACTIVE) {}

	// Planned Unit
	PlannedUnitt::PlannedUnitt(BlackCrow &parent, BWAPI::UnitType type) : Planned(parent), type(type) {}

	int PlannedUnitt::getMineralPrice() {
		return type.mineralPrice();
	}

	int PlannedUnitt::getGasPrice() {
		return type.gasPrice();
	}

	void PlannedUnitt::onFrame() {

	}

	// Planned Building
	PlannedBuilding::PlannedBuilding(BlackCrow &parent, BWAPI::UnitType type, BWAPI::TilePosition buildPosition) : Planned(parent), type(type), buildPosition(buildPosition) {}

	int PlannedBuilding::getMineralPrice() {
		return type.mineralPrice();
	}

	int PlannedBuilding::getGasPrice() {
		return type.gasPrice();
	}

	void PlannedBuilding::onFrame() {

	}

	// Planned Tech
	PlannedTech::PlannedTech(BlackCrow& parent, BWAPI::TechType type) : Planned(parent), type(type) {}

	int PlannedTech::getMineralPrice() {
		return type.mineralPrice();
	}

	int PlannedTech::getGasPrice() {
		return type.gasPrice();
	}

	void PlannedTech::onFrame() {

	}

	// Planned Upgrade
	PlannedUpgrade::PlannedUpgrade(BlackCrow& parent, BWAPI::UpgradeType type) : Planned(parent), type(type) {}

	int PlannedUpgrade::getMineralPrice() {
		return type.mineralPrice();
	}

	int PlannedUpgrade::getGasPrice() {
		return type.gasPrice();
	}

	void PlannedUpgrade::onFrame() {

	}

}