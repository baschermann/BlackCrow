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
		scoutSquads = new std::list<ScoutSquad*>();
		areas = new std::list<Area*>();
	}

	Strategy::~Strategy() {}

	void Strategy::onStart() {
		bo = getStartBuildOrder();
		currentBuildOrder = getBuildOrder(bo);

		// Area
		for (const BWEM::Area& area : bc.bwem.Areas()) {
			Area* ad = new Area(bc);
			ad->area = &area;
			areas->push_back(ad);
		}
	}

	void Strategy::update() {

		// Build the Buildorder
		if (currentBuildOrder->size() > 0) {
			Strategy::BuildOrderItem* boi = *currentBuildOrder->begin();

			// Check if there are enough ressources to build it
			if (bc.macro.getUnreservedMinerals() >= boi->type.mineralPrice()) {
				if (bc.macro.getNonReservedLarvaeAmount() > 0) {
					if (boi->type.isBuilding()) {
						bc.macro.planBuilding(boi->type, bc.macro.firstBase->hatchery->getPosition(), bc.macro.firstBase);
					} else {
						bc.macro.planUnit(boi->type, bc.macro.firstBase->hatchery->getPosition());
					}
					currentBuildOrder->pop_front();
				}
			}
		}
		else {

			// Dynamics time!
			if (currentBuildOrder->size() == 0) {
				bc.macro.autoBuildOverlords = true;
			}

			dynamicDecision();
		}

		if (scoutSquads->size() <= 0) {
			startInitialScout();
		}

		for (ScoutSquad* ss : *scoutSquads) {
			ss->onFrame();
		}
	}

	void Strategy::dynamicDecision() {

		// Lets do the macro first
		if (bc.macro.totalDronesNeeded() > 0) {
			if (bc.macro.getUnreservedMinerals() >= 50 && bc.macro.getNonReservedLarvaeAmount() > 0)
				bc.macro.buildDrone();
		}

		if (bc.macro.expansionNeeded() || bc.macro.getUnreservedMinerals() > 700) {
			//Broodwar << "Expansion needed" << std::endl;
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

		// Kruecke: This statement has been reached (in a test game) right after the last hatchery has died.
		// (In this case, that was just before the game was about to end. But that doesn't has to be the case necessarily.)
		assert(false);
		return nullptr;
	}

	void Strategy::onPlannedComplete(PlannedUnit plannedUnit) {

	}

	void Strategy::onPlannedDestroyed(PlannedUnit plannedUnit) {

	}

	void Strategy::onUnitDiscovered(BWAPI::Unit unit) {
		if (Broodwar->self()->isEnemy(unit->getPlayer())) {
			//Broodwar->sendText("Enemy Unit %s with id %i discovered!", unit->getType().c_str(), unit->getID());
			bc.enemy.enemyDiscovered(unit);
		}
	}
	void Strategy::onUnitDestroyed(BWAPI::Unit unit) {

	}

	Area* Strategy::getArea(int id) {
		for (Area* ad : *areas) {
			if (ad->area->Id() == id)
				return ad;
		}
		assert(false);
		return nullptr;
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
		ScoutSquad* ss = new ScoutSquad(bc);
		BWAPI::Unit worker = bc.macro.getDroneForBuilding(bc.macro.firstBase->hatchery->getPosition());
		assert(worker);
		SquadUnit* su = new SquadUnit(worker);
		ss->add(su);
		ss->addStartLocations();
		scoutSquads->push_back(ss);
	}

	std::list<Strategy::BuildOrderItem*>* Strategy::getBuildOrder(Strategy::BuildOrder build) {

		std::list<Strategy::BuildOrderItem*>* list = new std::list<Strategy::BuildOrderItem*>();

		switch (build) {
		case Strategy::BuildOrder::NINE_POOL:
		{
			{
				// Drone @4
				Strategy::BuildOrderItem* boi = new Strategy::BuildOrderItem();
				boi->type = BWAPI::UnitTypes::Zerg_Drone;
				list->push_back(boi);
			}
			
			{
			// Drone @5
			Strategy::BuildOrderItem* boi = new Strategy::BuildOrderItem();
			boi->type = BWAPI::UnitTypes::Zerg_Drone;
			list->push_back(boi);
			}
			
			{
			// Drone @6
			Strategy::BuildOrderItem* boi = new Strategy::BuildOrderItem();
			boi->type = BWAPI::UnitTypes::Zerg_Drone;
			list->push_back(boi);
			}

			{
			// Drone @7
			Strategy::BuildOrderItem* boi = new Strategy::BuildOrderItem();
			boi->type = BWAPI::UnitTypes::Zerg_Drone;
			list->push_back(boi);
			}

			{
			// Drone @8
			Strategy::BuildOrderItem* boi = new Strategy::BuildOrderItem();
			boi->type = BWAPI::UnitTypes::Zerg_Drone;
			list->push_back(boi);
			}
			
			{
			// Spawning Pool @9
			Strategy::BuildOrderItem* boi = new Strategy::BuildOrderItem();
			boi->type = BWAPI::UnitTypes::Zerg_Spawning_Pool;
			list->push_back(boi);
			}

			{
			// Drone @8
			Strategy::BuildOrderItem* boi = new Strategy::BuildOrderItem();
			boi->type = BWAPI::UnitTypes::Zerg_Drone;
			list->push_back(boi);
			}

			{
			// Extractor @9
			Strategy::BuildOrderItem* boi = new Strategy::BuildOrderItem();
			boi->type = BWAPI::UnitTypes::Zerg_Extractor;
			list->push_back(boi);
			}

			{
			// Overlord @8
			Strategy::BuildOrderItem* boi = new Strategy::BuildOrderItem();
			boi->type = BWAPI::UnitTypes::Zerg_Overlord;
			list->push_back(boi);
			}

			{
			// Drone @8
			Strategy::BuildOrderItem* boi = new Strategy::BuildOrderItem();
			boi->type = BWAPI::UnitTypes::Zerg_Drone;
			list->push_back(boi);
			}
			
			break;
		}
		case Strategy::BuildOrder::OVERPOOL:
		{
			int a = 4;
			break;
		}
		case Strategy::BuildOrder::TWELVE_HATCH:
		{
			int b = 3;
			break;
		}
		}

		return list;
	}
}