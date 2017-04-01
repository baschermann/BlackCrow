#include "Geyser.h"
#include "Worker.h"

namespace BlackCrow {
	Geyser::Geyser(const BWEM::Geyser* geyser) : bwemGeyser(geyser) {}

	bool Geyser::isMineable() {
		return geyserUnit && geyserUnit->exists() && geyserUnit->isCompleted() && bwemGeyser->Amount() > 0;
	}

	bool Geyser::workerNeeded() {
		return workers.size() < 3;
	}

	int Geyser::workersNeeded() {
		return 3 - workers.size();
	}

	void Geyser::registerWorker(Worker& worker) {
		workers.push_back(worker);
	}

	void Geyser::unregisterWorker(Worker& worker) {
		workers.erase(std::remove(workers.begin(), workers.end(), worker), workers.end());
	}
}