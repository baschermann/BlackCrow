#pragma once
#include <BWAPI.h>
#include <BWEM/bwem.h>
#include "Geyser.h"
#include "Mineral.h"

namespace BlackCrow {

	class BlackCrow;
	class Area;
	class Worker;
	class Geyser;
	class Mineral;

	class Base {
	public:
		Base(BlackCrow& parent, const BWEM::Base& bwemBase, Area& area);
		void onFrame();
		bool onUnitDestroyed(BWAPI::Unit unit);

		const BWEM::Base& bwemBase;
		BWAPI::Unit hatchery = nullptr;
		Area& area;
		bool isIsland;
		bool isExpanding = false;
		bool selfSustained = false;
		
		std::vector<Worker> workers;
		std::vector<Geyser> geysers;
		std::vector<Mineral> minerals;

		const bool isEstablished();
		void addWorker(BWAPI::Unit drone);
		BWAPI::Unit removeWorker();
		BWAPI::Unit removeWorker(BWAPI::Position closestTo);

		const bool workerNeeded();
		const int workersNeeded();
		const int totalWorkers();
		const int totalMineralWorkers();
		const int totalGasWorkers();

	private:
		BlackCrow& bc;
		Mineral& findMineralForWorker();
		const int gasWorkersNeeded();
	};
}