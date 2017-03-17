#pragma once
#include <BWAPI.h>

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

		class BuildOrderItem {
		public:
			BWAPI::UnitType type;
		};

		std::list<BuildOrderItem*>* currentBuildOrder;
		std::list<ScoutSquad*>* scoutSquads;
		std::list<Area*>* areas;
		BuildOrder bo;

		Strategy(BlackCrow &parent);
		~Strategy();
		void onStart();
		void update();
		std::list<BuildOrderItem*>* getBuildOrder(BuildOrder build);
		void onPlannedComplete(PlannedUnit plannedUnit);
		void onPlannedDestroyed(PlannedUnit plannedUnit);
		void onUnitDiscovered(BWAPI::Unit unit);
		void onUnitDestroyed(BWAPI::Unit unit);
		Area* getArea(int id);

	private:
		BlackCrow &bc;

		BuildOrder getStartBuildOrder();
		void buildDrone();
		BaseInformation* getSafestEstablishedBase();
		void dynamicDecision();
		void startInitialScout();
	};
}