#pragma once
#include <BWAPI.h>
#include <memory>

namespace BlackCrow {

	class BlackCrow;
	class Base;
	class Planned;
	class PlannedBuilding;
	class PlannedExtractor;

	struct Resources {
		int minerals;
		int gas;
	};

	class Macro {
	public:
		Macro(BlackCrow &parent);
		void onStart();
		void onFrame();

		// Variables
		std::vector<Base> bases;
		std::vector<std::shared_ptr<Planned>> plannedStuff;

		// Planned
		std::shared_ptr<PlannedUnit> planUnit(BWAPI::UnitType type);
		std::shared_ptr<PlannedBuilding> planBuilding(BWAPI::UnitType type, BWAPI::TilePosition buildPosition);
		std::shared_ptr<PlannedExtractor> planExtractor(BWAPI::UnitType type, BWAPI::TilePosition buildPosition);
		void onUnitComplete(BWAPI::Unit unit);
		
		// Expansions
		Base& getSafestToExpand();
		void expand();
		int getNumberOfEstablishedBases();
		int getNumberOfCurrentlyExpandingBases();
		Base& getSafestEstablishedBase();
		

		// Worker
		int totalWorkers();
		int mineralWorkers();
		int gasWorkers();
		bool workerNeededForSaturation();
		int workersNeededForSaturation();
		BWAPI::Unit getDroneForBuilding(BWAPI::Position position);

		// Gas
		int gasWorkerSlotsAvailable();
		int extractorsAbleToBuild(bool expansionsFinished);
		void buildExtractor();
		int extractorsCurrentlyBuilding();
		void addGasWorker();
		void removeGasWorker();

		// Larva
		BWAPI::Unit getUnreservedLarva();
		int getUnreservedLarvaeAmount();

		// Statistics and Numbers
		Resources getUnreservedResources();
		double getAverageMineralsPerFrame();
		double getAverageGasPerFrame();


	private:
		BlackCrow &bc;

		void initBases();
		BWAPI::Unit Macro::getStartingHatchery();
	};

}