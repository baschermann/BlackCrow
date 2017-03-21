#pragma once
#include <BWAPI.h>
#include <queue>

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
		std::list<ScoutSquad> scoutSquads;
		std::vector<Area> areas;
		BuildOrder bo;

		Strategy(BlackCrow &parent);
		~Strategy();
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