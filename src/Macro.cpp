#include "Macro.h"
#include "BlackCrow.h"
#include "Planned.h"
#include "Base.h"
#include "Worker.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Macro::Macro(BlackCrow &parent) : bc(parent) {}

	void Macro::onStart() {
		initBases();
	}

	void Macro::onFrame() {

	}

	// Planned
	std::shared_ptr<Planned> Macro::planUnit(BlackCrow& parent, BWAPI::UnitType type) {
		plannedStuff.push_back(std::make_shared<PlannedUnit>(parent, type));
	}

	std::shared_ptr<Planned> Macro::planBuilding(BlackCrow& parent, BWAPI::UnitType type, BWAPI::TilePosition buildPosition) {
		plannedStuff.push_back(std::make_shared<PlannedBuilding>(parent, type, buildPosition));
	}

	void Macro::onUnitComplete(BWAPI::Unit unit) {
		
	}

	// Expansions
	int Macro::numberOfEstablishedBases() {
		int amount = 0;
		for(Base& base : bases) {
			if (base.hatchery && base.hatchery->isCompleted())
				amount++;
		}
		return amount;
	}

	int Macro::numberOfCurrentExpansions() {

	}

	void Macro::expand() {

	}

	Base& Macro::getSafestExpansion() {

	}

	// Worker
	int Macro::totalWorkers() {
		int amount = 0;
		for (Base& base : bases) {
			for (Mineral& mineral : base.minerals) {
				amount += mineral.workers.size();
			}

			for (Geyser& geyser : base.geysers) {
				amount += geyser.workers.size();
			}
		}
		return amount;
	}

	int Macro::mineralWorkers() {
		int amount = 0;
		for (Base& base : bases) {
			for (Mineral& mineral : base.minerals) {
				amount += mineral.workers.size();
			}
		}
		return amount;
	}

	int Macro::gasWorkers() {
		int amount = 0;
		for (Base& base : bases) {
			for (Geyser& geyser : base.geysers) {
				amount += geyser.workers.size();
			}
		}
		return amount;
	}

	bool Macro::workerNeededForSaturation() {
		for (Base& base : bases) {
			if (base.workerNeeded())
				return true;
		}
		return false;
	}

	int Macro::workersNeededForSaturation() {
		int amount = 0;
		for (Base& base : bases) {
			amount += base.workersNeeded();
		}
		return amount;
	}


	// Gas
	int Macro::gasWorkerSlotsAvailable() {
		int amount = 0;
		for (Base& base : bases) {
			amount += base.workersNeeded();
		}
		return amount;
	}

	int Macro::extractorsAbleToBuild(bool expansionsFinished) {
		int amount = 0;
		for (Base& base : bases) {
			for (Geyser& geyser : base.geysers) {
				if (geyser.geyserUnit && geyser.geyserUnit->isCompleted())
					amount++;
			}
		}
		return amount;
	}

	void Macro::buildExtractor() {
		for (Base& base : bases) {
			for (Geyser& geyser : base.geysers) {
				if (!geyser.geyserUnit) {
					planBuilding(bc, BWAPI::UnitTypes::Zerg_Extractor, geyser.bwemGeyser->TopLeft());
					return;
				}
			}
		}
	}

	int Macro::extractorsCurrentlyBuilt() {
		int amount = 0;

		for (Base& base : bases) {
			for (Geyser& geyser : base.geysers) {
				if (geyser.geyserUnit && !geyser.geyserUnit->isCompleted()) {
					amount++;
				}
			}
		}
	}

	void Macro::addGasWorker() {

	}

	void Macro::removeGasWorker() {

	}


	// Estimates
	double Macro::averageMineralsPerFrame() {

	}

	double Macro::averageGasPerFrame() {

	}

	 // Private
	void Macro::initBases() {
		for (auto bwemArea : bc.bwem.Areas()) {
			auto area = bc.map.getArea(bwemArea);
			for (auto bwemBase : bwemArea.Bases()) {

				Base base(bc, bwemBase, area);

				// Starting Base
				if (bwemBase.Starting()) {
					BWAPI::Unit startingHatchery = getStartingHatchery();
					if (bwemBase.Location() == startingHatchery->getTilePosition()) {

						base.hatchery = startingHatchery;
						Unitset drones = Broodwar->getUnitsInRadius(startingHatchery->getPosition(), 350, BWAPI::Filter::IsWorker);

						for (Unit drone : drones) {
							base.addWorker(drone);
						}
					}
				}

				bases.push_back(base);
			}
		}
	}

	BWAPI::Unit Macro::getStartingHatchery() {
		for (auto &unit : Broodwar->self()->getUnits()) {
			if (unit->getType().isResourceDepot()) {
				return unit;
			}
		}
		return nullptr;
	}
}