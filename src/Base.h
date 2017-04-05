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

		const BWEM::Base& bwemBase;
		BWAPI::Unit hatchery = nullptr;
		Area& area;
		bool isIsland;
		bool isExpanding = false;
		bool selfSustained = false;
		
		std::vector<Geyser> geysers;
		std::vector<Mineral> minerals;

		bool isEstablished();
		void addWorker(BWAPI::Unit drone);
		BWAPI::Unit removeWorker();
		BWAPI::Unit removeWorker(BWAPI::Position closestTo);

		bool workerNeeded();
		int workersNeeded();
		std::vector<Worker> getMineralWorkers();
		std::vector<Worker> getGasWorkers();
		int totalMineralWorkers();
		int totalGasWorkers();
		int totalWorkers();

	private:
		BlackCrow& bc;
		Mineral& findMineralForWorker();
	};
}