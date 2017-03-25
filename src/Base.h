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
		Base(BlackCrow& parent, BWEM::Base& bwemBase, Area& area);
		void onFrame();

		BWEM::Base& bwemBase;
		BWAPI::Unit hatchery;
		Area& area;
		bool isIsland;
		bool isExpanding = false;
		std::vector<Geyser> geysers;
		std::vector<Mineral> minerals;

		void addWorker(Worker worker);
		std::unique_ptr<Worker> removeWorker();
		std::unique_ptr<Worker> removeWorker(BWAPI::Position closestTo);

		bool workerNeeded();
		int workersNeeded();
		std::vector<Worker> getMineralWorkers();
		std::vector<Worker> getGasWorkers();

	private:
		BlackCrow& bc;
		Mineral& findMineralForWorker();
		int totalMineralWorkers();
		int totalGasWorkers();
	};
}