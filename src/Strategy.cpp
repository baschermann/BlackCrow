#include "Strategy.h"
#include "BlackCrow.h"
#include <BWEM/bwem.h>
#include "Area.h"
#include "Planned.h"
#include "Macro.h"


namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Strategy::Strategy(BlackCrow &parent) : bc(parent) {
		unitMix = std::make_unique<UnitMix>(bc);
	}

	void Strategy::onStart() {
		fillBuildOrder(getStartBuildOrder());

		unitMix->set(BWAPI::UnitTypes::Zerg_Drone, 1);
	}

	void Strategy::onFrame() {

		if (buildOrder.size() > 0) {
			followBuildOrder();
		} else {
			dynamicDecision();
		}

		if (!initialScoutStarted && bc.macro.getUsedSupply() >= 18) {
			bc.army.startInitialScout();
			initialScoutStarted = true;
		}
	}

	void Strategy::followBuildOrder() {
		UnitType type = buildOrder.front();
		Resources unreservedResources = bc.macro.getUnreservedResources();

		if (unreservedResources.minerals >= type.mineralPrice() && unreservedResources.gas >= type.gasPrice()) {
			if (type.isBuilding()) {
				if (type == UnitTypes::Zerg_Extractor) {
					bc.macro.buildExtractor();
				} else if (type == UnitTypes::Zerg_Hatchery) {
					bc.macro.expand();
				} else {
					TilePosition buildPosition = bc.builder.getBuildingSpot(type, false);
					if (buildPosition != TilePositions::None)
						bc.macro.planBuilding(type, buildPosition);
				}
			} else {
				if (bc.macro.getUnreservedLarvaeAmount() > 0)
					bc.macro.planUnit(type, bc.macro.startPosition);
			}
			buildOrder.pop_front();
		}
	}

	void Strategy::dynamicDecision() {
		
		// Check if we need drones, add them to the mix
		if (bc.macro.getWorkersNeededForSaturation() - bc.macro.getCurrentlyPlannedAmount(UnitTypes::Zerg_Drone) > 0 || bc.macro.getTotalWorkerAmount() >= bc.config.maxDrones) {
			if (!unitMix->exists(UnitTypes::Zerg_Drone))
				unitMix->set(UnitTypes::Zerg_Drone, 1);
		} else {
			if (unitMix->exists(UnitTypes::Zerg_Drone))
				unitMix->remove(UnitTypes::Zerg_Drone);
		}

		// Collect up to 100 gas
		if (Broodwar->self()->gas() + Broodwar->self()->spentGas() < 100) {
			if (bc.macro.getGasWorkerSlotsAvailable() > 0)
				if (bc.macro.getTotalGasWorkerAmount() < 3)
					bc.macro.addGasWorker();
		} else {
			if (bc.macro.getTotalGasWorkerAmount() > 0)
				bc.macro.removeGasWorker();
		}

		// Check if we have a spawning pool
		auto ownUnits = Broodwar->self()->getUnits();
		if (std::find_if(ownUnits.begin(), ownUnits.end(), [](Unit unit) { return unit->getType() == UnitTypes::Zerg_Spawning_Pool; }) != ownUnits.end() && !bc.macro.getCurrentlyPlannedAmount(BWAPI::UnitTypes::Zerg_Spawning_Pool)) {

			// Zergling Speed
			if (bc.macro.getUnreservedResources().gas >= 100 && !bc.macro.isCurrentlyPlanned(UpgradeTypes::Metabolic_Boost) && Broodwar->self()->getUpgradeLevel(UpgradeTypes::Metabolic_Boost) <= 0) {
				bc.macro.planUpgrade(UpgradeTypes::Metabolic_Boost, 1);
			}

			// Building Zerglings
			if (!unitMix->exists(UnitTypes::Zerg_Zergling))
				unitMix->set(UnitTypes::Zerg_Zergling, 1);
		}
		
		if (unitMix->size() > 0) {

			productionMultiplierMinerals = bc.macro.getAverageMineralsPerFrame() / unitMix->mineralPerFrame();
			//productionMultiplierGas = bc.macro.getAverageGasPerFrame() / unitMix->gasPerFrame(); // TODO Gas units not included
			productionMultiplierLarvae = bc.macro.getAverageLarvaePerFrame() / unitMix->larvaPerFrame();
			productionMultiplier = std::min(productionMultiplierMinerals, productionMultiplierLarvae);

			// Supply first
			if (bc.macro.getFreeSupply() / (unitMix->supplyPerFrame() * productionMultiplier) < UnitTypes::Zerg_Overlord.buildTime() && bc.macro.getFreeSupply() <= 20) {
				bc.macro.planUnit(UnitTypes::Zerg_Overlord, bc.macro.startPosition);
			}

			// Drone
			if (unitMix->peek() == UnitTypes::Zerg_Drone) {
				if (bc.macro.getUnreservedResources().minerals >= 50 && bc.macro.getUnreservedLarvaeAmount() > 0) {
					bc.macro.buildWorkerDrone();
					unitMix->pop();
				}
					
			// Combat  Units
			} else {
				if (bc.macro.getUnreservedResources().minerals >= unitMix->peek().mineralPrice() && bc.macro.getUnreservedLarvaeAmount() > 0)
					bc.macro.planUnit(unitMix->pop(), bc.macro.startPosition);
			}

			// Additional Hatcheries
			if (productionMultiplierLarvae < productionMultiplierMinerals && bc.macro.getTotalLarvaeAmount() <= 0) {
				int amountPlannedHatcheries = bc.macro.getCurrentlyPlannedAmount(UnitTypes::Zerg_Hatchery);

				if (amountPlannedHatcheries <= 0) {
					if (bc.macro.getUnreservedResources().minerals >= 250)
						bc.macro.planBuilding(UnitTypes::Zerg_Hatchery, bc.builder.getBuildingSpot(UnitTypes::Zerg_Hatchery));
				} else {
					if ((double)bc.macro.getUnreservedResources().minerals / ((double)amountPlannedHatcheries * (double)300) >= 0.75)
						bc.macro.planBuilding(UnitTypes::Zerg_Hatchery, bc.builder.getBuildingSpot(UnitTypes::Zerg_Hatchery));
				}
			}
		}
	}

	Strategy::BuildOrder Strategy::getStartBuildOrder() {
		if (Broodwar->enemy()->getRace().getName() == "Terran") {
			return Strategy::BuildOrder::TWELVE_HATCH;
		} else {
			if (Broodwar->enemy()->getRace().getName() == "Zerg") {
				return Strategy::BuildOrder::NINE_POOL;
			} else {
				if (Broodwar->enemy()->getRace().getName() == "Protoss") {
					return Strategy::BuildOrder::OVERPOOL;
				}
			}
		}
		return Strategy::BuildOrder::OVERPOOL;
	}


	void Strategy::fillBuildOrder(BuildOrder build) {

		switch (build) {
		case Strategy::BuildOrder::NINE_POOL:

			// Drone @4
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Drone);
			// Drone @5
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Drone);
			// Drone @6
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Drone);
			// Drone @7
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Drone);
			// Drone @8
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Drone);
			// Spawning Pool @9
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Spawning_Pool);
			// Drone @8
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Drone);
			// Extractor @9
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Extractor);
			// Overlord @8
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Overlord);
			// Drone @8
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Drone);
			// 6 Zerglings @8
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Zergling);
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Zergling);
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Zergling);

			break;

		case Strategy::BuildOrder::OVERPOOL:

			// Drone @4
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Drone);
			// Drone @5
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Drone);
			// Drone @6
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Drone);
			// Drone @7
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Drone);
			// Drone @8
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Drone);
			// Overlord @9
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Overlord);
			// Spawning Pool @9
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Spawning_Pool);
			// Drone @8
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Drone);
			// Drone @9
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Drone);
			// Drone @10
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Drone);
			// Extractor @11
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Extractor);
			// 6 Zerglings @10
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Zergling);
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Zergling);
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Zergling);

			break;

		case Strategy::BuildOrder::TWELVE_HATCH:

			// Drone @4
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Drone);
			// Drone @5
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Drone);
			// Drone @6
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Drone);
			// Drone @7
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Drone);
			// Drone @8
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Drone);
			// Overlord @9
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Overlord);
			// Drone @9
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Drone);
			// Drone @10
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Drone);
			// Drone @11
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Drone);
			// Drone @12
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Drone);
			// Hatchery @12
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Drone);
			// Spawning Pool @11
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Spawning_Pool);
			// Extractor @11
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Extractor);
			// 6 Zerglings @10
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Zergling);
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Zergling);
			fillBuildOrderItem(BWAPI::UnitTypes::Zerg_Zergling);

			break;

		}
	}

	void Strategy::fillBuildOrderItem(UnitType item) {
		buildOrder.emplace_back(item);
	}
}