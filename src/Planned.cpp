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
	PlannedUnit::PlannedUnit(BlackCrow &parent, BWAPI::UnitType type, BWAPI::Position nearTo) : Planned(parent), type(type), nearTo(nearTo) {}

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
			unit = bc.macro.getUnreservedLarva(nearTo);
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

		// Is it finished?
		if (unit && unit->getType() == type && unit->isCompleted()) {
			status = Status::COMPLETED;

			if (type == UnitTypes::Zerg_Drone) {
				bc.macro.addDrone(unit);
			} else {
				if (type != UnitTypes::Zerg_Overlord) {
					bc.army.assignAutomaticSquad(bc.army.addToArmy(unit));
				}
			}
		}
	}

	BWAPI::Unit PlannedUnit::reservedLarva() {
		/*if (unit && unit->getType() == UnitTypes::Zerg_Larva)
			return unit;
		else
			return nullptr;*/
		return unit;
	}

	float PlannedUnit::progressPercent() {
		if (unit) {
			if (unit->getType() == UnitTypes::Zerg_Egg) {
				return ((float)unit->getRemainingBuildTime() / (float)type.buildTime());
			}
		}
		return 0;
	}

	std::string PlannedUnit::getName() {
		return type.getName();
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
			if (!droneOrBuilding)
				return;
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

	float PlannedBuilding::progressPercent() {
		if (droneOrBuilding && droneOrBuilding->getType().isBuilding()) {
			return ((float)droneOrBuilding->getRemainingBuildTime() / (float)droneOrBuilding->getType().buildTime());
		}
		return 0;
	}

	std::string PlannedBuilding::getName() {
		return type.getName();
	}



	// Planned Extractor
	PlannedExtractor::PlannedExtractor(BlackCrow &parent, Geyser& geyser) : Planned(parent), geyser(geyser) {
		geyser.registerPlannedExtractor(*this);
	}

	int PlannedExtractor::getMineralPrice() {
		if (!alreadyBuiltExtractor)
			return UnitTypes::Zerg_Extractor.mineralPrice();
		else
			return 0;
	}

	int PlannedExtractor::getGasPrice() {
		if (!alreadyBuiltExtractor)
			return UnitTypes::Zerg_Extractor.gasPrice();
		else
			return 0;
	}

	void PlannedExtractor::onFrame() {
		if (status == Status::FAILED || status == Status::COMPLETED) {
			geyser.unregisterPlannedExtractor(*this);
			return;
		}

		if (!drone)
			drone = bc.macro.getDroneForBuilding(geyser.bwemGeyser->Pos());

		if (drone && drone->exists()) {
			if(drone->build(UnitTypes::Zerg_Extractor, geyser.bwemGeyser->TopLeft()))
				alreadyBuiltExtractor = true;
		} else if(alreadyBuiltExtractor) {
			if (!geyser.geyserUnit || !geyser.geyserUnit->exists()) {
				auto unitsOnGeyser = Broodwar->getUnitsOnTile(geyser.bwemGeyser->TopLeft(), IsRefinery && IsBuilding);
				if (unitsOnGeyser.size() > 0) {
					geyser.geyserUnit = *unitsOnGeyser.begin();
					geyserUnitFound = true;
				}

				if (unitsOnGeyser.size() > 1)
					Broodwar->sendText("Found more than 1 building for extractor");
			} else if (geyser.geyserUnit->isCompleted()) {
				status = Status::COMPLETED;
			}
		}

		if ((status == Status::ACTIVE && drone && !drone->exists() && !alreadyBuiltExtractor) // Drone died during building
			|| (status == Status::ACTIVE && alreadyBuiltExtractor && geyserUnitFound && !geyser.geyserUnit->exists()) // Extractor was killed while constructing
			) {
			status = Status::FAILED;
			return;
		}
	}
	
	float PlannedExtractor::progressPercent() {
		if (geyser.geyserUnit) {
			return ((float)geyser.geyserUnit->getRemainingBuildTime() / (float)geyser.geyserUnit->getType().buildTime());
		}
		return 0;
	}

	std::string PlannedExtractor::getName() {
		return UnitTypes::Zerg_Extractor.getName();
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
		Broodwar->sendText("PlannedTech onFrame() not implemented");
	}

	float PlannedTech::progressPercent() {
		return 0;
	}

	std::string PlannedTech::getName() {
		return type.getName();
	}

	// Planned Upgrade
	PlannedUpgrade::PlannedUpgrade(BlackCrow& parent, BWAPI::UpgradeType type, int level) : Planned(parent), type(type), level(level) {}

	int PlannedUpgrade::getMineralPrice() {
		if (researchingBuilding && researchingBuilding->isUpgrading())
			return 0;
		else
			return type.mineralPrice();
	}

	int PlannedUpgrade::getGasPrice() {
		if (researchingBuilding && researchingBuilding->isUpgrading())
			return 0;
		else
			return type.gasPrice();
	}

	void PlannedUpgrade::onFrame() {
		if (researchingBuilding && !researchingBuilding->exists()) {
			status = Status::FAILED;
			return;
		}

		if (status == Status::ACTIVE && !researchingBuilding) {
			auto ownUnits = Broodwar->self()->getUnits();
			auto freeSpawningPoolIt = std::find_if(ownUnits.begin(), ownUnits.end(), [](BWAPI::Unit unit) { return unit->getType() == UnitTypes::Zerg_Spawning_Pool && !unit->isUpgrading(); });
			if (freeSpawningPoolIt != ownUnits.end()) {
				researchingBuilding = *freeSpawningPoolIt;
			}
		}

		if (status == Status::ACTIVE && researchingBuilding)
			if (!researchingBuilding->isUpgrading() && researchingBuilding->canUpgrade())
				if (Broodwar->self()->minerals() >= type.mineralPrice() && Broodwar->self()->gas() >= type.gasPrice())
					researchingBuilding->upgrade(type);

		if (status == Status::ACTIVE && Broodwar->self()->getUpgradeLevel(type) >= level)
			status = Status::COMPLETED;
		
	}

	float PlannedUpgrade::progressPercent() {
		if (researchingBuilding && researchingBuilding->isUpgrading())
			return (float)researchingBuilding->getRemainingUpgradeTime() / (float)type.upgradeTime(level);
		else
			return 0;
	}

	std::string PlannedUpgrade::getName() {
		return type.getName();
	}

}