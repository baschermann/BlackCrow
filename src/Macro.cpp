#include "Macro.h"
#include "BlackCrow.h"
#include "Planned.h"
#include "Worker.h"
#include <numeric>

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Macro::Macro(BlackCrow &parent) : bc(parent) {}

	void Macro::onStart() {
		initBases();
		startPosition = getStartingHatchery()->getPosition();
	}

	void Macro::onFrame() {
		for (auto it = plannedStuff.begin(); it != plannedStuff.end();) {
			std::shared_ptr<Planned> planned = *it;
			planned->onFrame();
			if (planned->getStatus() == Planned::Status::FAILED || planned->getStatus() == Planned::Status::COMPLETED) {
				it = plannedStuff.erase(it);
			} else {
				it++;
			}
		}

		for (Base& base : bases)
			base.onFrame();
	}


	void Macro::onUnitCompleted(BWAPI::Unit unit) {
		if (unit->getType() == UnitTypes::Zerg_Hatchery)
			hatcheries.push_back(unit);
	}

	void Macro::onUnitDestroyed(BWAPI::Unit unit) {
		if (unit->getType() == UnitTypes::Zerg_Hatchery || unit->getType() == UnitTypes::Zerg_Lair || unit->getType() == UnitTypes::Zerg_Hive)
			hatcheries.erase(std::remove(hatcheries.begin(), hatcheries.end(), unit), hatcheries.end());

		for (Base& base : bases) {
			if (base.onUnitDestroyed(unit))
				break;
		}
	}

	// ### Planned ###
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
		return std::accumulate(plannedStuff.begin(), plannedStuff.end(), 0, [&](int sum, std::shared_ptr<Planned> planned) { 

			if (type == UnitTypes::Zerg_Extractor) {
				// TODO typeof comparison here?
				std::shared_ptr<PlannedExtractor> plannedExtractor = std::dynamic_pointer_cast<PlannedExtractor>(planned);
				if (plannedExtractor)
					return ++sum;
			}

			std::shared_ptr<PlannedUnit> plannedUnit = std::dynamic_pointer_cast<PlannedUnit>(planned);
			if (plannedUnit && plannedUnit->type == type)
				return ++sum;

			std::shared_ptr<PlannedBuilding> plannedBuilding = std::dynamic_pointer_cast<PlannedBuilding>(planned);
			if (plannedBuilding && plannedBuilding->type == type)
				return ++sum;

			std::shared_ptr<PlannedUpgrade> plannedUpgrade = std::dynamic_pointer_cast<PlannedUpgrade>(planned);
			if (plannedUpgrade && plannedUpgrade->type == type)
				return ++sum;

			std::shared_ptr<PlannedTech> plannedTech = std::dynamic_pointer_cast<PlannedTech>(planned);
			if (plannedTech && plannedTech->type == type)
				return ++sum;
			
			return sum;
		});
	}

	std::vector<std::shared_ptr<PlannedUnit>> Macro::getPlannedUnits() {
		std::vector<std::shared_ptr<PlannedUnit>> plannedUnits;

		for (auto planned : plannedStuff) {
			std::shared_ptr<PlannedUnit> plannedUnit = std::dynamic_pointer_cast<PlannedUnit>(planned);
			if (plannedUnit)
				plannedUnits.push_back(plannedUnit);
		}

		return plannedUnits;
	}

	// ### Expansions and Bases ### // TODO Need enemy information to do this
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
		for (Base& base : bases) {
			if (base.isEstablished())
				return base;
		}
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
	int Macro::getTotalWorkerAmount() {
		return std::accumulate(bases.begin(), bases.end(), 0, [](int sum, Base& base) { return sum + base.getTotalWorkers(); });
	}

	int Macro::getTotalMineralWorkerAmount() {
		return std::accumulate(bases.begin(), bases.end(), 0, [](int sum, Base& base) { return sum + base.getTotalMineralWorkers(); });
	}

	int Macro::getTotalGasWorkerAmount() {
		return std::accumulate(bases.begin(), bases.end(), 0, [](int sum, Base& base) { return sum + base.getTotalGasWorkers(); });
	}

	bool Macro::isWorkerNeededForSaturation() {
		return std::any_of(bases.begin(), bases.end(), [](Base& base) { return base.isWorkerNeeded(); });
	}

	int Macro::getWorkersNeededForSaturation() {
		return std::accumulate(bases.begin(), bases.end(), 0, [](int sum, Base& base) { return sum + base.getWorkersNeeded(); });
	}
	
	void Macro::buildWorkerDrone() {
		auto base = std::find_if(bases.begin(), bases.end(), [](Base& base) { return base.isWorkerNeeded(); });
		planUnit(UnitTypes::Zerg_Drone, base->bwemBase.Center());
	}

	void Macro::addDrone(BWAPI::Unit drone) {
		Base* closestEstablished = nullptr;
		double closestEstablishedDistance = std::numeric_limits<double>::max();

		Base* closestEstablishedWorkerNeeded = nullptr;
		double closestEstablishedWorkerNeededDistance = std::numeric_limits<double>::max();

		for (Base& base : bases) {
			if (base.isEstablished()) {

				// Get any base where we can put a worker if we have no base that needs a worker
				if (!closestEstablished) {
					closestEstablished = &base;
					closestEstablishedDistance = Util::distance(closestEstablished->bwemBase.Center(), drone->getPosition());
				}

				// If the base needs worker and is closer, use it
				if (base.isWorkerNeeded()) {
					double distance = Util::distance(base.bwemBase.Center(), drone->getPosition());
					if (distance < closestEstablishedWorkerNeededDistance) {
						closestEstablishedWorkerNeeded = &base;
						closestEstablishedWorkerNeededDistance = distance;
					}
				}

				// If we have no closest base that needs worker, check if the current established base is closest and if yes, use it
				if (!closestEstablishedWorkerNeeded) {
					double distance = Util::distance(base.bwemBase.Center(), drone->getPosition());
					if (distance < closestEstablishedDistance) {
						closestEstablishedDistance = distance;
						closestEstablished = &base;
					}
				}
			}
		}

		if (closestEstablishedWorkerNeeded)
			closestEstablishedWorkerNeeded->addWorker(drone);
		else if(closestEstablished)
			closestEstablished->addWorker(drone);
			// If not, I guess we are dead
	}

	BWAPI::Unit Macro::getDroneForBuilding(BWAPI::Position position) {
		// Predicates for getNearestBase, we want to have workers here
		return getNearestBase(position).removeWorker(position);
	}


	// Gas
	int Macro::getGasWorkerSlotsAvailable() {
		return std::accumulate(bases.begin(), bases.end(), 0, [](int sum, Base& base) { return sum + base.getGasWorkerSlotsAvailable(); });
	}

	int Macro::getExtractorsAbleToBuild() {
		return std::accumulate(bases.begin(), bases.end(), 0, [&](int sum, Base& base) { return sum + base.getExtractorsAbleToBuild(); });
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

	bool Macro::addGasWorker() {
		if (getGasWorkerSlotsAvailable() > 0) {
			auto base = find_if(bases.begin(), bases.end(), [](Base& base) { return base.getGasWorkerSlotsAvailable() > 0; });
			return base->shiftWorkerToGas();
		}
		Broodwar->sendText("No gas worker slots available");
		return false;
	}

	void Macro::removeGasWorker() {
		if (getTotalGasWorkerAmount() > 0) {
			auto base = find_if(bases.begin(), bases.end(), [](Base& base) { return base.getTotalGasWorkers() > 0; });
			base->shiftWorkerFromGas();
		}
	}

	// Larva
	int Macro::getTotalLarvaeAmount() {
		return getAllLarvae().size();
	}

	// Can return nullptr
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
		Resources resources = { Broodwar->self()->minerals(), Broodwar->self()->gas() };

		for (std::shared_ptr<Planned> planned : plannedStuff) {
			resources.minerals -= planned->getMineralPrice();
			resources.gas -= planned->getGasPrice();
		}

		return resources;
	}

	int Macro::getUsedSupply() {
		return Broodwar->self()->supplyUsed();
	}

	int Macro::getMaxSupply() {
		return getTypeCurrentlyPlanned(UnitTypes::Zerg_Overlord) * UnitTypes::Zerg_Overlord.supplyProvided() + Broodwar->self()->supplyTotal();
	}

	int Macro::getFreeSupply() {
		return getMaxSupply() - getUsedSupply();
	}

	int Macro::getPlannedSupply() {
		auto plannedUnits = getPlannedUnits();
		return std::accumulate(plannedUnits.begin(), plannedUnits.end(), 0, [](int sum, std::shared_ptr<PlannedUnit> plannedUnit) { return sum + plannedUnit->type.supplyRequired(); });
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
				bases.emplace_back(bc, bwemBase, area);
				Base& base = bases.back();

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
			}
		}
		
		for (Base& base : bases)
			if (base.hatchery)
				for (Worker& worker : base.workers)
					if (worker.mineral && worker.mineral->id <= 0) {
						// TODO BIG BIG ERROR
						//assert(!"Wrong mineral ids!");
						Broodwar->sendText("!!!!! Error! Wrong Mineral IDs! !!!!!");
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
			std::shared_ptr<PlannedUnit> pu = std::dynamic_pointer_cast<PlannedUnit>(planned); // TODO Dynamic Cast is bad?
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
		std::vector<BWAPI::Unit> reservedLarvae = getReservedLarvae();
		std::vector<BWAPI::Unit> unreservedLarvae;

		std::sort(allLarvae.begin(), allLarvae.end());
		std::sort(reservedLarvae.begin(), reservedLarvae.end());
		std::set_difference(allLarvae.begin(), allLarvae.end(), reservedLarvae.begin(), reservedLarvae.end(), std::inserter(unreservedLarvae, unreservedLarvae.begin()));

		return unreservedLarvae;
	}
}