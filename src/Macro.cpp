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
		for (std::shared_ptr<Planned> planned : plannedStuff) {
			planned->onFrame();
		}
	}


	void Macro::onUnitCompleted(BWAPI::Unit unit) {
		if (unit->getType() == UnitTypes::Zerg_Hatchery)
			hatcheries.push_back(unit);
	}

	void Macro::onUnitDestroyed(BWAPI::Unit unit) {
		if (unit->getType() == UnitTypes::Zerg_Hatchery || unit->getType() == UnitTypes::Zerg_Lair || unit->getType() == UnitTypes::Zerg_Hive)
			hatcheries.erase(std::remove(hatcheries.begin(), hatcheries.end(), unit), hatcheries.end());
	}

	// Planned
	std::shared_ptr<PlannedUnit> Macro::planUnit(BWAPI::UnitType type, BWAPI::Position nearTo) {
		auto unit = std::make_shared<PlannedUnit>(bc, type, nearTo);
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

	int Macro::getTypeCurrentlyPlanned(BWAPI::UnitType type) {
		return 0;
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
	
	void Macro::buildWorkerDrone() {

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

	// Larva
	int Macro::getTotalLarvaeAmount() {
		return 0;
	}

	BWAPI::Unit Macro::getUnreservedLarva(Position nearTo) {
		std::vector<BWAPI::Unit> url = getUnreservedLarvae();
		if (url.size() > 0)
			return Util::findClosestUnit(url, nearTo);
		else
		return nullptr;
	}

	int Macro::getUnreservedLarvaeAmount() {
		return getUnreservedLarvae().size();
	}


	// Estimates
	Resources Macro::getUnreservedResources() {
		Resources resources = { 0, 0 };

		for (std::shared_ptr<Planned> planned : plannedStuff) {
			resources.minerals += planned->getMineralPrice();
			resources.gas += planned->getGasPrice();
		}

		return resources;
	}

	double Macro::getAverageMineralsPerFrame() {
		return 0;
	}

	double Macro::getAverageGasPerFrame() {
		return 0;
	}

	// Private
	void Macro::initBases() {
		for (const BWEM::Area& bwemArea : bc.bwem.Areas()) {
			
			Area& area = bc.map.getArea(bwemArea);
			for (const BWEM::Base& bwemBase : bwemArea.Bases()) {
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

	std::vector<BWAPI::Unit> Macro::getAllLarvae() {
		std::vector<BWAPI::Unit> allLarvae;

		for (Unit hatchery : hatcheries) {
			Unitset hatchLarvae = hatchery->getLarva();
			std::copy(hatchLarvae.begin(), hatchLarvae.end(), std::back_inserter(allLarvae));
		}
		return allLarvae;
	}

	std::vector<BWAPI::Unit> Macro::getReservedLarvae() {
		std::vector<BWAPI::Unit> reservedLarvae;

		// TODO this doesn't look good
		for (std::shared_ptr<Planned> planned : plannedStuff) {
			PlannedUnit* pu = dynamic_cast<PlannedUnit*>(planned.get()); // TODO Dynamic Cast is bad?
			if (pu) {
				Unit larva = pu->reservedLarva();
				if (larva)
					reservedLarvae.push_back(larva); // Can you push a nullptr?
			}
		}

		return reservedLarvae;
	}

	std::vector<BWAPI::Unit> Macro::getUnreservedLarvae() {
		std::vector<BWAPI::Unit> allLarvae = getAllLarvae();
		//Broodwar->sendText("Total Larvae: %i", allLarvae.size());
		std::vector<BWAPI::Unit> reservedLarvae = getReservedLarvae();
		//Broodwar->sendText("Total Larvae: %i", reservedLarvae.size());
		std::vector<BWAPI::Unit> unreservedLarvae;

		std::sort(allLarvae.begin(), allLarvae.end());
		std::sort(reservedLarvae.begin(), reservedLarvae.end());
		std::set_difference(allLarvae.begin(), allLarvae.end(), reservedLarvae.begin(), reservedLarvae.end(), std::inserter(unreservedLarvae, unreservedLarvae.begin()));

		return unreservedLarvae;
	}
}