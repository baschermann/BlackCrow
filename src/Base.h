#pragma once
#include <BWAPI.h>
#include <BWEM/bwem.h>
#include "Worker.h"
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

		BWEM::Base& bwemBase;
		BWAPI::Unit hatchery;
		Area& area;
		bool isIsland;
		bool isExpanding = false;
		std::vector<Geyser> geysers;
		std::vector<Mineral> minerals;

		void addWorker(Worker worker);
		Worker removeWorker();
		Worker removeWorker(BWAPI::Position closestTo);

		bool workerNeeded();
		int workersNeeded();

	private:
		BlackCrow& bc;
		Mineral& findMineralForWorker();
	};
}