#pragma once
#include <BWAPI.h>
#include <memory>
#include <deque>
#include "Base.h"
#include "Common.h"

namespace BlackCrow {

	class BlackCrow;
	class Base;
	class Planned;
	class PlannedUnit;
	class PlannedBuilding;
	class PlannedExtractor;
	class PlannedTech;
	class Geyser;

	struct Resources {
		int minerals;
		int gas;

		Resources operator+(Resources& other) {
			minerals += other.minerals;
			gas += other.gas;
			return *this;
		}
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
		PlannedUnitPtr planUnit(BWAPI::UnitType type, BWAPI::Position nearTo);
		PlannedBuildingPtr planBuilding(BWAPI::UnitType type, BWAPI::TilePosition buildPosition);
		PlannedExtractorPtr planExtractor(Geyser& geyser);
		PlannedUpgradePtr planUpgrade(BWAPI::UpgradeType upgrade, int level = 1);
		PlannedTechPtr planTech(BWAPI::TechType tech);
		int getCurrentlyPlannedAmount(BWAPI::UnitType type);
		bool isCurrentlyPlanned(BWAPI::UpgradeType upgrade, int level = 1);
		bool isCurrentlyPlanned(BWAPI::TechType tech);
		std::vector<std::shared_ptr<PlannedUnit>> getPlannedUnits();
		void removePlanned(std::shared_ptr<Planned> planned);
		
		// Expansions and Bases
		Base* getSafestToExpand();
		void expand();
		int getNumberOfCurrentlyExpandingBases();
		int getNumberOfEstablishedBases();
		Base* getSafestEstablishedBase();
		Base* getNearestBase(BWAPI::Position position);
		
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
		int hasAmountOf(BWAPI::UnitType type);
		Resources getUnreservedResources();
		int getUsedSupply();
		int getMaxSupply();
		int getFreeSupply();
		int getPlannedSupply();
		
		double getAverageLarvaePerFrame();
		double getAverageGasPerFrame();
		double getAverageMineralsPerFrame();



		double mineralFrameAverage = 0;
		double gasFrameAverage = 0;

		

	private:
		BlackCrow &bc;

		void initBases();
		BWAPI::Unit Macro::getStartingHatchery();
		std::vector<BWAPI::Unit> getAllLarvae();
		std::vector<BWAPI::Unit> getReservedLarvae();
		std::vector<BWAPI::Unit> getUnreservedLarvae();

		void calculateResourceAverages();
		// Minerals
		std::deque<int> mineralIncomeList;
		int lastFrameTotalMinerals = 0;
		double averageMineralsPerFrame = 0;
		// Gas
		std::deque<int> gasIncomeList;
		int lastFrameTotalGas = 0;
		double averageGasPerFrame = 0;
	};

}