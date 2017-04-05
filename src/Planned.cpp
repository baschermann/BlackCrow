#include "Planned.h"
#include "BlackCrow.h"
#include <BWEM/bwem.h>
#include "Geyser.h"
#include "Macro.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Planned::Planned(BlackCrow &parent) : bc(parent), status(Status::ACTIVE) {}

	// Planned Unit
	PlannedUnit::PlannedUnit(BlackCrow &parent, BWAPI::UnitType type) : Planned(parent), type(type) {}

	int PlannedUnit::getMineralPrice() {
		if (!unit || unit->getType() == UnitTypes::Zerg_Larva)
			return type.mineralPrice();
		else
			return 0;
	}

	int PlannedUnit::getGasPrice() {
		if (!unit || unit->getType() == UnitTypes::Zerg_Larva)
			return type.gasPrice();
		else
			return 0;
	}

	void PlannedUnit::onFrame() {
		if (status == Status::FAILED)
			return;

		// Was unit/larva/egg killed?
		if (status == Status::ACTIVE && unit && !unit->exists() && alreadyGrabbedLarva) {
			status = Status::FAILED;
			return;
		}

		// When we have no larva, grab one
		if (!unit) {
			unit = bc.macro.getUnreservedLarva();
			if (unit) {
				alreadyGrabbedLarva = true;
			} else {
				return;
			}
		}

		// Can we morph it?
		if (unit && unit->getType() == UnitTypes::Zerg_Larva)
			if (Broodwar->self()->minerals() >= type.mineralPrice() && Broodwar->self()->gas() >= type.gasPrice())
				unit->morph(type);
	}

	BWAPI::Unit PlannedUnit::reservedLarva() {
		if (unit && unit->getType() == UnitTypes::Zerg_Larva)
			return unit;
		else
			return nullptr;
	}

	// Planned Building
	PlannedBuilding::PlannedBuilding(BlackCrow &parent, BWAPI::UnitType type, BWAPI::TilePosition buildPosition) : Planned(parent), type(type), buildPosition(buildPosition) {
		if (type == UnitTypes::Zerg_Extractor)
			assert(!"Wrong Building Type. Use PlannedExtractor instead of PlannedBuilding for building an extractor");
	}

	int PlannedBuilding::getMineralPrice() {
		if (droneOrBuilding && droneOrBuilding->getType().isBuilding())
			return 0;
		else
			return type.mineralPrice();
	}

	int PlannedBuilding::getGasPrice() {
		if (droneOrBuilding && droneOrBuilding->getType().isBuilding())
			return 0;
		else
			return type.gasPrice();
	}

	void PlannedBuilding::onFrame() {
		if (status == Status::FAILED || status == Status::COMPLETED)
			return;

		if (!droneOrBuilding) {
			droneOrBuilding = bc.macro.getDroneForBuilding(Position(buildPosition));
		}

		if (status == Status::ACTIVE && droneOrBuilding && !droneOrBuilding->exists()) {
			status = Status::FAILED;
			return;
		}

		if (!droneOrBuilding->getType().isBuilding()) {
			Position buildingCenter = Position(buildPosition) + Util::middleOfBuilding(type);

			if (Util::distance(droneOrBuilding->getPosition(), buildingCenter) <= BWAPI::UnitTypes::Zerg_Drone.sightRange() * 0.8) {
				if (Broodwar->self()->minerals() >= type.mineralPrice() && Broodwar->self()->gas() >= type.gasPrice()) {
					if (!droneOrBuilding->build(type, buildPosition)) { // Check instead if drone is busy or something?

						// TODO Error Spam
						if (Broodwar->getLastError() != BWAPI::Errors::Unit_Busy) {
							Broodwar << Broodwar->getLastError() << std::endl;
						}
					}
				}
			} else {
				droneOrBuilding->move(buildingCenter);
			}
		} else {
			if (droneOrBuilding->isCompleted())
				status = Status::COMPLETED;
		}
	}

	// Planned Extractor
	PlannedExtractor::PlannedExtractor(BlackCrow &parent, Geyser& geyser) : Planned(parent), geyser(geyser) {
		geyser.registerPlannedExtractor(*this);
	}

	int PlannedExtractor::getMineralPrice() {
		return UnitTypes::Zerg_Extractor.mineralPrice();
	}

	int PlannedExtractor::getGasPrice() {
		return UnitTypes::Zerg_Extractor.gasPrice();
	}

	void PlannedExtractor::onFrame() {
		if (status == Status::FAILED || status == Status::COMPLETED) {
			geyser.unregisterPlannedExtractor(*this);
			return;
		}

		if (!drone) {
			drone = bc.macro.getDroneForBuilding(geyser.bwemGeyser->Pos());
			alreadyGrabbedDrone = true;
		}

		if (drone) {
			drone->build(UnitTypes::Zerg_Extractor, geyser.bwemGeyser->TopLeft());
			alreadyBuiltExtractor = true;
		} else if(alreadyBuiltExtractor && !drone->exists()) {
			if (!geyser.geyserUnit || !geyser.geyserUnit->exists()) {
				auto unitsOnGeyser = Broodwar->getUnitsOnTile(geyser.bwemGeyser->TopLeft(), IsRefinery && IsBuilding);
				if (unitsOnGeyser.size() > 0)
					geyser.geyserUnit = *unitsOnGeyser.begin();

				if (unitsOnGeyser.size() > 1)
					Broodwar->sendText("Found more than 1 building for extractor");
			} else if (geyser.geyserUnit->isCompleted()) {
				status = Status::COMPLETED;
			}
		}

		if (status == Status::ACTIVE && alreadyGrabbedDrone && alreadyBuiltExtractor && drone && !drone->exists()) {
			status = Status::FAILED;
			return;
		}
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