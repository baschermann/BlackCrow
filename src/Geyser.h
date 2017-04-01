#pragma once
#include <BWAPI.h>
#include <BWEM/bwem.h>

namespace BlackCrow {

	class Worker;

	class Geyser {
	public:
		Geyser(const BWEM::Geyser* geyser);

		const BWEM::Geyser* bwemGeyser = nullptr;
		std::vector<Worker> workers;
		BWAPI::Unit geyserUnit = nullptr;

		bool isMineable();
		bool workerNeeded();
		int workersNeeded();
		void registerWorker(Worker& worker);
		//Worker removeWorker();
		void unregisterWorker(Worker& worker);
	};
}

