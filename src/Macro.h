#pragma once
#include <BWAPI.h>
#include <memory>
#include "Base.h"

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
		void onUnitCompleted(BWAPI::Unit unit);
		void onUnitDestroyed(BWAPI::Unit unit);

		// Variables
		BWAPI::Position startPosition;
		std::vector<Base> bases;
		std::vector<std::shared_ptr<Planned>> plannedStuff;
		std::vector<BWAPI::Unit> hatcheries;

		// Planned
		std::shared_ptr<PlannedUnit> planUnit(BWAPI::UnitType type, BWAPI::Position nearTo);
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
		int getTotalWorkerAmount();
		int getTotalMineralWorkerAmount();
		int getTotalGasWorkerAmount();
		bool isWorkerNeededForSaturation();
		int getWorkersNeededForSaturation();
		void buildWorkerDrone();
		void addDrone(BWAPI::Unit drone);
		BWAPI::Unit getDroneForBuilding(BWAPI::Position position);

		// Gas
		int getGasWorkerSlotsAvailable();
		int getExtractorsAbleToBuild();
		void buildExtractor();
		bool addGasWorker();
		void removeGasWorker();

		// Larva
		int getTotalLarvaeAmount();
		BWAPI::Unit getUnreservedLarva(BWAPI::Position nearTo);
		int getUnreservedLarvaeAmount();

		// Statistics and Numbers
		Resources getUnreservedResources();
		double getAverageMineralsPerFrame();
		double getAverageGasPerFrame();


	private:
		BlackCrow &bc;

		void initBases();
		BWAPI::Unit Macro::getStartingHatchery();
		std::vector<BWAPI::Unit> getAllLarvae();
		std::vector<BWAPI::Unit> getReservedLarvae();
		std::vector<BWAPI::Unit> getUnreservedLarvae();
		
	};

}