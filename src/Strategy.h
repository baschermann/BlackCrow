#pragma once
#include <BWAPI.h>
#include <queue>
#include "Area.h"
#include "Squad.h"

namespace BlackCrow {

	class BlackCrow;
	class PlannedUnit;
	class BaseInformation;
	class ScoutSquad;
	class Area;

	class Strategy {

	public:

		enum BuildOrder {
			NINE_POOL,
			OVERPOOL,
			TWELVE_HATCH
		};

		std::queue<BWAPI::UnitType> buildOrder;
		std::vector<ScoutSquad> scoutSquads;
		std::vector<Area> areas;
		BuildOrder bo;

		Strategy(BlackCrow &parent);
		void onStart();
		void update();
		void onPlannedComplete(PlannedUnit& plannedUnit);
		void onPlannedDestroyed(PlannedUnit& plannedUnit);
		void onUnitDiscovered(BWAPI::Unit unit);
		void onUnitDestroyed(BWAPI::Unit unit);

	private:
		BlackCrow &bc;

		BuildOrder getStartBuildOrder();
		void fillBuildOrder(BuildOrder build);
		void fillBuildOrderItem(BWAPI::UnitType item);
		void buildDrone();
		BaseInformation* getSafestEstablishedBase();
		void dynamicDecision();
		void startInitialScout();
	};
}