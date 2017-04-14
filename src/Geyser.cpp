#include "Geyser.h"
#include "Planned.h"

namespace BlackCrow {
	Geyser::Geyser(const BWEM::Geyser* geyser) : bwemGeyser(geyser) {}

	bool Geyser::isMineable() {
		return geyserUnit && geyserUnit->exists() && geyserUnit->isCompleted() && bwemGeyser->Amount() > 0;
	}

	bool Geyser::isBuildable() {
		if (!geyserUnit || !geyserUnit->exists())
			return true;
		return false;
	}

	bool Geyser::isCurrentlyBuilding() {
		if (plannedExtractor && plannedExtractor->getStatus() == Planned::Status::ACTIVE)
			return true;
		return false;
	}

	bool Geyser::workerNeeded() {
		return workers.size() < 3;
	}

	int Geyser::workersNeeded() {
		return 3 - workers.size();
	}

	void Geyser::registerWorker(Worker& worker) {
		workers.push_back(&worker);
		int a = 9*9;
	}

	void Geyser::unregisterWorker(Worker& worker) {
		workers.erase(std::remove(workers.begin(), workers.end(), &worker), workers.end());
		int b = 9 * 10;
	}
	
	void Geyser::registerPlannedExtractor(PlannedExtractor& pu) {
		plannedExtractor = &pu;
	}

	void Geyser::unregisterPlannedExtractor(PlannedExtractor& pu) {
		if (&pu == plannedExtractor)
			plannedExtractor = nullptr;
	}
}