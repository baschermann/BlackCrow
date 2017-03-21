#pragma once
#include <BWAPI.h>
#include <BWEM/bwem.h>
#include <iostream>
#include <assert.h>
#include <algorithm>

namespace BlackCrow {

	// Handles Workers and getting Expansions. Decide how to build them, not when (that is the task of the Priority Manager)

	class BlackCrow;
	class PlannedUnit;

	class BaseInformation {
	public:
		BWAPI::Unit hatchery;
		BWAPI::Unit extractor;
		BWAPI::Unitset workersOnMinerals;
		BWAPI::Unitset workersOnGas;
		const BWEM::Base* base;
		bool underAttack;
		PlannedUnit* plannedExpansion;
		bool isExpanding;
		bool isSafe;
	};

	class PlannedUnit {
	public:

		enum PlanType { UNIT, BUILDING, BUILDING_WITH_LOCATION, EXPANSION };

		PlanType planType;
		BWAPI::UnitType type;
		BWAPI::Unit larvaEgg;
		BWAPI::Unit drone;
		BaseInformation* base;
		BWAPI::Position requestPosition;
		BWAPI::TilePosition extractorTilePosition;
		BWAPI::TilePosition buildLocation;
		bool buildLocationChosen;
		bool isMacroHatchery;
	};

	class Macro {

	public:
		std::list<BaseInformation*> *bases;
		std::list<PlannedUnit*>* plannedUnits;
		BWAPI::Unitset* macroHatcheries;
		BWAPI::Unitset* reservedLarvae;
		int supplyUnderway;
		bool autoBuildOverlords;
		BaseInformation* firstBase;

		Macro(BlackCrow &bc);
		void onStart();
		void update();
		void onUnitComplete(BWAPI::Unit unit);
		void onUnitDestroyed(BWAPI::Unit unit);
		BWAPI::Unit getStartingHatchery();
		std::list<BaseInformation*>* Macro::getAllBaseInformation();
		BaseInformation* getStartBaseInformation();
		void updateGasMineralsRatio();
		void updateExpansionBuilding();
		void updateShiftWorkers();
		BaseInformation* findClosestMiningBase(BWAPI::Position position);
		BaseInformation* findClosestMiningBaseWithWorkers(BWAPI::Position position);
		BaseInformation* getClosestExpansionLocation(BWAPI::TilePosition position);


		PlannedUnit* Macro::planUnit(BWAPI::UnitType unitType, BWAPI::Position position);
		PlannedUnit* Macro::planBuilding(BWAPI::UnitType unitType, BWAPI::Position position, BaseInformation* base);
		PlannedUnit* Macro::planBuilding(BWAPI::UnitType unitType, BWAPI::Position position, BaseInformation* base, BWAPI::TilePosition buildLocation);
		PlannedUnit* Macro::planExpansion(BWAPI::UnitType unitType, BWAPI::Position position, BaseInformation* base, BWAPI::TilePosition buildLocation);
		int getReservedMinerals();
		int getUnreservedMinerals();
		int totalDronesNeeded();
		bool expansionNeeded();
		bool buildDrone();
		void buildExpansion();
		BWAPI::Unitset* getLarvae();
		int getLarvaeAmount();
		int getNonReservedLarvaeAmount();
		BWAPI::Unitset* getNonReservedLarvae();
		BWAPI::Unit findClosestLarva(BWAPI::Position);
		BWAPI::Unit getNonReservedLarva(BWAPI::Position position);
		int getNumberOfHatcheryBases();
		int getNumberOfMacroHatcheries();
		BWAPI::Unit getDroneForBuilding(BWAPI::Position position);
		PlannedUnit* Macro::buildMacroHatchery(BaseInformation* base);
		int amountMacroHatcheriesBeingBuilt();
		int amountHatcheriesBeingBuilt();

	private:
		BlackCrow &bc;

		PlannedUnit* plan(BWAPI::UnitType unitType, BWAPI::Position position);
		int dronesCurrentlyNeeded(BaseInformation* base);
		int dronesTotalNeeded(BaseInformation* base);

	};
}