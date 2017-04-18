#pragma once
#include <BWAPI.h>
#include <BWEM/bwem.h>
#include "Worker.h"

namespace BlackCrow {

	class Worker;
	class PlannedExtractor;

	class Geyser {
	public:
		Geyser(const BWEM::Geyser* geyser);

		const BWEM::Geyser* bwemGeyser = nullptr;
		std::vector<std::shared_ptr<Worker>> workers;
		BWAPI::Unit geyserUnit = nullptr;
		

		bool isMineable();
		bool isBuildable();
		bool isCurrentlyBuilding();
		bool workerNeeded();
		int workersNeeded();
		void registerWorker(std::shared_ptr<Worker> worker);
		void unregisterWorker(std::shared_ptr<Worker> worker);
		void registerPlannedExtractor(PlannedExtractor& pu);
		void unregisterPlannedExtractor(PlannedExtractor& pu);

	private:
		PlannedExtractor* plannedExtractor = nullptr;
	};
}

