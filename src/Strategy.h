#pragma once
#include <BWAPI.h>
#include <list>
#include "Squad.h" // Can this be moved to .cpp?
#include "UnitMix.h" // Needed for UnitMixPtr which is a unique_ptr, can this be moved to .cpp?

namespace BlackCrow {

	class BlackCrow;
	class PlannedUnit;
	class BaseInformation;
	class ScoutSquad;
	class Area;
	class Planned;
	class UnitMix;

	class Strategy {
	public:

		enum class BuildOrder {
			NINE_POOL,
			OVERPOOL,
			TWELVE_HATCH
		};

		// General
		Strategy(BlackCrow &parent);
		void onStart();
		void onFrame();
		void dynamicDecision();

		// Build Order
		std::list<BWAPI::UnitType> buildOrder;
		BuildOrder bo;

		// Unit Mix
		double droneProbability = 1; // 1 = 100%
		UnitMixPtr unitMix = nullptr;

		// Decision decisions
		double productionMultiplierMinerals = 0;
		double productionMultiplierGas = 0;
		double productionMultiplierLarvae = 0;
		double productionMultiplier = 0;

	private:
		BlackCrow &bc;
		bool initialScoutStarted = false;

		// Buildorder
		BuildOrder getStartBuildOrder();
		void fillBuildOrder(BuildOrder build);
		void fillBuildOrderItem(BWAPI::UnitType item);
		void followBuildOrder();
	};
}