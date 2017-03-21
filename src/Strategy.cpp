#include "Strategy.h"
#include "BlackCrow.h"
#include <BWEM/bwem.h>
#include "Area.h"
#include "Squad.h"
#include "SquadUnit.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Strategy::Strategy(BlackCrow &parent) : bc(parent) {
	}

	Strategy::~Strategy() {}

	void Strategy::onStart() {
		fillBuildOrder(getStartBuildOrder());

		// Area
		for (const BWEM::Area& bwemArea : bc.bwem.Areas()) {
			Area ad(bc, bwemArea);
			areas.push_back(ad);
		}
	}

	void Strategy::update() {

		// Build the Buildorder
		if (buildOrder.size() > 0) {
			UnitType type = buildOrder.front();

			// Check if there are enough ressources to build it
			if (bc.macro.getUnreservedMinerals() >= type.mineralPrice()) {
				if (bc.macro.getNonReservedLarvaeAmount() > 0) {
					if (type.isBuilding()) {
						if (type == UnitTypes::Zerg_Hatchery)
							bc.macro.buildExpansion();
						else {
							assert(bc.macro.firstBase->hatchery); // TODO Crash when the first hatchery is being destroyed and the build order wants to build a building
							bc.macro.planBuilding(type, bc.macro.firstBase->hatchery->getPosition(), bc.macro.firstBase);
						}
					} else {
						bc.macro.planUnit(type, bc.macro.firstBase->hatchery->getPosition());
					}
					buildOrder.pop();
				}
			}
		} else {

			// Dynamics time!
			if (buildOrder.size() == 0) {
				bc.macro.autoBuildOverlords = true;
			}

			dynamicDecision();
		}

		if (scoutSquads.size() <= 0) {
			startInitialScout();
		}

		for (ScoutSquad& ss : scoutSquads) {
			ss.onFrame();
		}
	}

	void Strategy::dynamicDecision() {

		// Lets do the macro first
		if (bc.macro.totalDronesNeeded() > 0) {
			if (bc.macro.getUnreservedMinerals() >= 50 && bc.macro.getNonReservedLarvaeAmount() > 0)
				bc.macro.buildDrone();
		}

		if (bc.macro.expansionNeeded() || bc.macro.getUnreservedMinerals() > 400) {
			bc.macro.buildExpansion();
		}

		if (bc.macro.getUnreservedMinerals() >= 300 && bc.macro.getLarvaeAmount() <= 0) {
			if (bc.macro.amountMacroHatcheriesBeingBuilt() < 1) {
				bc.macro.buildMacroHatchery(getSafestEstablishedBase());
			}
		}
	}

	BaseInformation* Strategy::getSafestEstablishedBase() {
		// TODO get the safest base depending where the fight is
		// for now just get the first base with a hatchery

		for (BaseInformation* baseInformation : *bc.macro.bases) {
			if (baseInformation->hatchery) {
				return baseInformation;
			}
		}

		// TODO FIX wenn keine basis
		return *(bc.macro.bases->begin());
	}

	void Strategy::onPlannedComplete(PlannedUnit& plannedUnit) {

	}

	void Strategy::onPlannedDestroyed(PlannedUnit& plannedUnit) {

	}

	void Strategy::onUnitDiscovered(BWAPI::Unit unit) {
		if (Broodwar->self()->isEnemy(unit->getPlayer())) {
			bc.enemy.enemyDiscovered(unit);
		}
	}

	void Strategy::onUnitDestroyed(BWAPI::Unit unit) {

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


	void Strategy::startInitialScout() {
		ScoutSquad scoutSquad(bc);
		BWAPI::Unit worker = bc.macro.getDroneForBuilding(bc.macro.firstBase->hatchery->getPosition());
		assert(worker);

		SquadUnit su(worker);
		scoutSquad.add(su);

		scoutSquad.addStartLocations();
		scoutSquads.push_back(scoutSquad);

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
			// Now has 11 Drones

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
		buildOrder.emplace(item);
	}
}