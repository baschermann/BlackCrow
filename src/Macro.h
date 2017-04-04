#pragma once
#include <BWAPI.h>
#include <memory>

namespace BlackCrow {

	class BlackCrow;
	class Base;
	class Planned;
	class PlannedUnit;
	class PlannedBuilding;
	class PlannedExtractor;
	class Geyser;

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
		BWAPI::Position startPosition;
		std::vector<Base> bases;
		std::vector<std::shared_ptr<Planned>> plannedStuff;

		// Planned
		std::shared_ptr<PlannedUnit> planUnit(BWAPI::UnitType type);
		std::shared_ptr<PlannedBuilding> planBuilding(BWAPI::UnitType type, BWAPI::TilePosition buildPosition);
		std::shared_ptr<PlannedExtractor> planExtractor(Geyser& geyser);
		int getTypeCurrentlyPlanned(BWAPI::UnitType type);
		
		// Expansions and Bases
		Base& getSafestToExpand();
		void expand();
		int getNumberOfCurrentlyExpandingBases();
		int getNumberOfEstablishedBases();
		Base& getSafestEstablishedBase();
		Base& getNearestBase(BWAPI::Position position);
		
		// Worker
		int getTotalWorkers();
		int getMineralWorkers();
		int getGasWorkers();
		bool isWorkerNeededForSaturation();
		int getWorkersNeededForSaturation();
		void buildWorkerDrone();
		BWAPI::Unit getDroneForBuilding(BWAPI::Position position);

		// Gas
		int getGasWorkerSlotsAvailable();
		int getExtractorsAbleToBuild(bool expansionsFinished);
		void buildExtractor();
		int getExtractorsCurrentlyBuilding();
		void addGasWorker();
		void removeGasWorker();

		// Larva
		int getTotalLarvaeAmount();
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