#pragma once
#include <BWAPI.h>
#include "MacroManager.h"
#include "Squad.h"
#include "SquadUnit.h"
#include "Mapdata.h"
#include "AreaData.h"
#include "Enemy.h"
#include "BuildingManager.h"


class StrategyManager {

public:

	enum BuildOrder {
		NINE_POOL,
		OVERPOOL,
		TWELVE_HATCH
	};

	class BuildOrderItem {
	public:
		BWAPI::UnitType type;
	};

	std::list<BuildOrderItem*>* currentBuildOrder;
	std::list<ScoutSquad*>* scoutSquads;
	std::list<AreaData*>* areas;
	BuildOrder bo;
	
	StrategyManager();
	~StrategyManager();
	void onStart();
	void update();
	std::list<BuildOrderItem*>* getBuildOrder(BuildOrder build);
	void onPlannedComplete(PlannedUnit plannedUnit);
	void onPlannedDestroyed(PlannedUnit plannedUnit);
	void onUnitDiscovered(BWAPI::Unit unit);
	void onUnitDestroyed(BWAPI::Unit unit);
	AreaData* StrategyManager::getAreaData(int id);

private:
	BuildOrder getStartBuildOrder();
	void buildDrone();
	BaseInformation* getSafestEstablishedBase();
	void dynamicDecision();
	void startInitialScout();
};

extern StrategyManager* strategyManager;