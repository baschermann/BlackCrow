#include "Mineral.h"
#include <algorithm>
#include "Worker.h"

namespace BlackCrow {
	Mineral::Mineral(const BWEM::Mineral* mineral) : bwemMineral(mineral) {
		assert(mineral->Unit()->exists());
		id = mineral->Unit()->getID();
	}

	bool Mineral::exists() {
		return bwemMineral != nullptr;
	}

	void Mineral::registerWorker(Worker& worker) {
		workers.push_back(&worker);
	}

	void Mineral::unregisterWorker(Worker& worker) {
		workers.erase(std::remove(workers.begin(), workers.end(), &worker), workers.end());
	}

	bool operator==(const Mineral& left, const Mineral& right) {
		return left.id == right.id;
	}
}