#pragma once
#include <BWAPI.h>
#include <BWEM/bwem.h>

namespace BlackCrow {

	class Worker;

	class Geyser {
	public:
		Geyser(BWEM::Geyser& geyser);

		BWEM::Geyser& bwemGeyser;
		std::vector<Worker> workers;
		BWAPI::Unit geyserUnit;

		bool isMineable();
		bool workerNeeded();
		int workersNeeded();
		void addWorker(Worker& worker);
		Worker removeWorker();
		void removeWorker(Worker& worker);
	};
}

