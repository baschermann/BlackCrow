#include "Macro.h"
#include "BlackCrow.h"
#include "Planned.h"
#include "Worker.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Macro::Macro(BlackCrow &parent) : bc(parent) {}

	void Macro::onStart() {
		initBases();
		startPosition = getStartingHatchery()->getPosition();
	}

	void Macro::onFrame() {

	}

	// Planned
	std::shared_ptr<PlannedUnit> Macro::planUnit(BWAPI::UnitType type) {
		auto unit = std::make_shared<PlannedUnit>(bc, type);
		plannedStuff.push_back(unit);
		return unit;
	}

	std::shared_ptr<PlannedBuilding> Macro::planBuilding(BWAPI::UnitType type, BWAPI::TilePosition buildPosition) {
		auto building = std::make_shared<PlannedBuilding>(bc, type, buildPosition);
		plannedStuff.push_back(building);
		return building;
	}

	std::shared_ptr<PlannedExtractor> Macro::planExtractor(Geyser& geyser) {
		auto extractor = std::make_shared<PlannedExtractor>(bc, geyser);
		plannedStuff.push_back(extractor);
		return extractor;
	}

	// Expansions and Bases // TODO Need enemy information to do this
	Base& Macro::getSafestToExpand() {
		return bases.front();
	}

	void Macro::expand() {

	}

	int Macro::getNumberOfCurrentlyExpandingBases() {
		return 0;
	}

	int Macro::getNumberOfEstablishedBases() {
		return 0;
	}

	Base& Macro::getSafestEstablishedBase() {
		return bases.front();
	}
	// TODO end

	Base& Macro::getNearestBase(BWAPI::Position position) { // Is there a better, or templatey/lambda way?
		Base* closestBase = nullptr;
		double closestDistance = std::numeric_limits<double>::max();
		for(Base& base : bases) {
			if (base.isEstablished()) {
				double distance = Util::distance(base.bwemBase.Center(), position);
				if (distance < closestDistance) {
					closestDistance = distance;
					closestBase = &base;
				}
			}
		}
		return *closestBase;
	}

	// Worker
	int Macro::getTotalWorkers() {
		int amount = 0;
		for (Base& base : bases) {
			amount += base.totalWorkers();
		}
		return amount;
	}

	int Macro::getMineralWorkers() {
		int amount = 0;
		for (Base& base : bases) {
			amount += base.totalMineralWorkers();
		}
		return amount;
	}

	int Macro::getGasWorkers() {
		int amount = 0;
		for (Base& base : bases) {
			amount += base.totalGasWorkers();
		}
		return amount;
	}

	bool Macro::isWorkerNeededForSaturation() {
		for (Base& base : bases) {
			if (base.workerNeeded())
				return true;
		}
		return false;
	}

	int Macro::getWorkersNeededForSaturation() {
		int amount = 0;
		for (Base& base : bases) {
			amount += base.workersNeeded();
		}
		return amount;
	}

	BWAPI::Unit Macro::getDroneForBuilding(BWAPI::Position position) {
		return getNearestBase(position).removeWorker(position);
	}


	// Gas
	int Macro::getGasWorkerSlotsAvailable() {
		int amount = 0;
		for (Base& base : bases) {
			amount += base.workersNeeded();
		}
		return amount;
	}

	int Macro::getExtractorsAbleToBuild(bool expansionsFinished) {
		int amount = 0;
		for (Base& base : bases) {
			if (base.isEstablished()) {
				for (Geyser& geyser : base.geysers) {
					if (geyser.isBuildable())
						amount++;
				}
			}
		}
		return amount;
	}

	void Macro::buildExtractor() { // TODO build extractor at safeset base
		for (Base& base : bases) {
			if (base.isEstablished()) {
				for (Geyser& geyser : base.geysers) {
					if (geyser.isBuildable()) {
						planExtractor(geyser);
							return;
					}
				}
			}
		}
	}

	int Macro::getExtractorsCurrentlyBuilding() {
		int amount = 0;

		for (Base& base : bases) {
			for (Geyser& geyser : base.geysers) {
				if (geyser.isCurrentlyBuilding()) {
					amount++;
				}
			}
		}
		return amount;
	}

	void Macro::addGasWorker() {

	}

	void Macro::removeGasWorker() {

	}


	// Estimates
	double Macro::getAverageMineralsPerFrame() {
		return 0;
	}

	double Macro::getAverageGasPerFrame() {
		return 0;
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