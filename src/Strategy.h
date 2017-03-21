#pragma once
#include <BWAPI.h>
#include <queue>
#include <memory>
#include "Squad.h"

namespace BlackCrow {

	class BlackCrow;
	class PlannedUnit;
	class BaseInformation;
	class ScoutSquad;
	class Area;
	class Planned;

	class Strategy {

	public:

		enum BuildOrder {
			NINE_POOL,
			OVERPOOL,
			TWELVE_HATCH
		};

		std::queue<BWAPI::UnitType> buildOrder;
		std::vector<ScoutSquad> scoutSquads;
		std::vector<std::shared_ptr<Planned>> plannedStuff;
		BuildOrder bo;

		Strategy(BlackCrow &parent);
		void onStart();
		void update();
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