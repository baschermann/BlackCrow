#include "StrategyManager.h"

using namespace BWAPI;
using namespace Filter;
//namespace { BWEM::Map* map; }

StrategyManager* strategyManager = new StrategyManager();

StrategyManager::StrategyManager() {
	scoutSquads = new std::list<ScoutSquad*>();
	areas = new std::list<AreaData*>();
}

StrategyManager::~StrategyManager() {}

void StrategyManager::onStart() {
	//map = &BWEM::Map::Instance();
	bo = getStartBuildOrder();
	currentBuildOrder = getBuildOrder(bo);

	// AreaData
	for (const BWEM::Area& area : BWEM::Map::Instance().Areas()) {
		AreaData* ad = new AreaData();
		ad->area = &area;
		areas->push_back(ad);
	}
}

void StrategyManager::update() {

	// Build the Buildorder
	if (currentBuildOrder->size() > 0) {
		StrategyManager::BuildOrderItem* boi = *currentBuildOrder->begin();

		// Check if there are enough ressources to build it
		if (macroManager->getUnreservedMinerals() >= boi->type.mineralPrice()) {
			if (macroManager->getNonReservedLarvaeAmount() > 0) {
				if (boi->type.isBuilding()) {
					macroManager->planBuilding(boi->type, macroManager->firstBase->hatchery->getPosition(), macroManager->firstBase);
				} else {
					macroManager->planUnit(boi->type, macroManager->firstBase->hatchery->getPosition());
				}
				currentBuildOrder->pop_front();
			}
		}
	} else {
		
		// Dynamics time!
		if (currentBuildOrder->size() == 0) {
			macroManager->autoBuildOverlords = true;
		}

		dynamicDecision();

		if (scoutSquads->size() <= 0) {
			startInitialScout();
		}
	}

	for (ScoutSquad* ss : *scoutSquads) {
		ss->onFrame();
	}
}

void StrategyManager::dynamicDecision() {
	
	// Lets do the macro first
	if (macroManager->totalDronesNeeded() > 0) {
		if (macroManager->getUnreservedMinerals() >= 50 && macroManager->getNonReservedLarvaeAmount() > 0)
			macroManager->buildDrone();
	}

	if (macroManager->expansionNeeded() || macroManager->getUnreservedMinerals() > 700) {
		//Broodwar << "Expansion needed" << std::endl;
		macroManager->buildExpansion();
	}

	if (macroManager->getUnreservedMinerals() >= 300 && macroManager->getLarvaeAmount() <= 0) {
		if (macroManager->amountMacroHatcheriesBeingBuilt() < 1) {
			macroManager->buildMacroHatchery(getSafestEstablishedBase());
		}
	}
}

BaseInformation* StrategyManager::getSafestEstablishedBase() {
	// TODO get the safest base depending where it is being attacked
	// for now just get the first base with a hatchery

	for (BaseInformation* baseInformation : *macroManager->bases) {
		if (baseInformation->hatchery) {
			return baseInformation;
		}
	}
	assert(false);
	return nullptr;
}

void StrategyManager::onPlannedComplete(PlannedUnit plannedUnit) {
	
}

void StrategyManager::onPlannedDestroyed(PlannedUnit plannedUnit) {

}

void StrategyManager::onUnitDiscovered(BWAPI::Unit unit) {
	if(Broodwar->self()->isEnemy(unit->getPlayer())) {
		//Broodwar->sendText("Enemy Unit %s with id %i discovered!", unit->getType().c_str(), unit->getID());
		enemy->enemyDiscovered(unit);
	}
}
void StrategyManager::onUnitDestroyed(BWAPI::Unit unit) {

}


AreaData* StrategyManager::getAreaData(int id) {
	for (AreaData* ad : *areas) {
		if (ad->area->Id() == id)
			return ad;
	}
	assert(false);
	return nullptr;
}

StrategyManager::BuildOrder StrategyManager::getStartBuildOrder() {
	// Which Race is the Enemy Playing
	if (Broodwar->enemy()->getRace().getName() == "Terran") {
		//Broodwar << "Terran Enemy Detected" << std::endl;
		return StrategyManager::BuildOrder::TWELVE_HATCH;
	}
	else {
		if (Broodwar->enemy()->getRace().getName() == "Zerg") {
			//Broodwar << "Zerg Enemy Detected" << std::endl;
			return StrategyManager::BuildOrder::NINE_POOL;
		}
		else {
			if (Broodwar->enemy()->getRace().getName() == "Protoss") {
				//Broodwar << "Protoss Enemy Detected" << std::endl;
				return StrategyManager::BuildOrder::OVERPOOL;
			}
		}
	}
	return StrategyManager::BuildOrder::OVERPOOL;
}


void StrategyManager::startInitialScout() {
	ScoutSquad* ss = new ScoutSquad();
	BWAPI::Unit worker = macroManager->getDroneForBuilding(macroManager->firstBase->hatchery->getPosition());
	assert(worker);
	SquadUnit* su = new SquadUnit(worker);
	ss->add(su);
	ss->addStartLocations();
	scoutSquads->push_back(ss);
}

std::list<StrategyManager::BuildOrderItem*>* StrategyManager::getBuildOrder(StrategyManager::BuildOrder build) {

	std::list<StrategyManager::BuildOrderItem*>* list = new std::list<StrategyManager::BuildOrderItem*>();

	switch (build) {
		case StrategyManager::BuildOrder::NINE_POOL:
		{
			{
				// Drone @4
				StrategyManager::BuildOrderItem* boi = new StrategyManager::BuildOrderItem();
				boi->type = BWAPI::UnitTypes::Zerg_Drone;
				list->push_back(boi);
			}
			
			/*
			{
				// Drone @5
				StrategyManager::BuildOrderItem* boi = new StrategyManager::BuildOrderItem();
				boi->type = BWAPI::UnitTypes::Zerg_Drone;
				list->push_back(boi);
			}

			{
				// Drone @6
				StrategyManager::BuildOrderItem* boi = new StrategyManager::BuildOrderItem();
				boi->type = BWAPI::UnitTypes::Zerg_Drone;
				list->push_back(boi);
			}
			
			{
				// Drone @7
				StrategyManager::BuildOrderItem* boi = new StrategyManager::BuildOrderItem();
				boi->type = BWAPI::UnitTypes::Zerg_Drone;
				list->push_back(boi);
			}

			{
				// Drone @8
				StrategyManager::BuildOrderItem* boi = new StrategyManager::BuildOrderItem();
				boi->type = BWAPI::UnitTypes::Zerg_Drone;
				list->push_back(boi);
			}
			
			{
				// Spawning Pool @9
				StrategyManager::BuildOrderItem* boi = new StrategyManager::BuildOrderItem();
				boi->type = BWAPI::UnitTypes::Zerg_Spawning_Pool;
				list->push_back(boi);
			}
			
			
			{
				// Drone @8
				StrategyManager::BuildOrderItem* boi = new StrategyManager::BuildOrderItem();
				boi->type = BWAPI::UnitTypes::Zerg_Drone;
				list->push_back(boi);
			}

			{
				// Extractor @9
				StrategyManager::BuildOrderItem* boi = new StrategyManager::BuildOrderItem();
				boi->type = BWAPI::UnitTypes::Zerg_Extractor;
				list->push_back(boi);
			}

			{
				// Overlord @8
				StrategyManager::BuildOrderItem* boi = new StrategyManager::BuildOrderItem();
				boi->type = BWAPI::UnitTypes::Zerg_Overlord;
				list->push_back(boi);
			}
	
			{
				// Drone @8
				StrategyManager::BuildOrderItem* boi = new StrategyManager::BuildOrderItem();
				boi->type = BWAPI::UnitTypes::Zerg_Drone;
				list->push_back(boi);
			}
			*/
			break;
		}
		case StrategyManager::BuildOrder::OVERPOOL:
		{
			int a = 4;
			break;
		}
		case StrategyManager::BuildOrder::TWELVE_HATCH:
		{
			int b = 3;
			break;
		}
	}

	return list;
}