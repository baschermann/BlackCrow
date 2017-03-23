#include "Mineral.h"
#include <algorithm>
#include "Worker.h"

namespace BlackCrow {
	Mineral::Mineral(BWEM::Mineral& mineral) : bwemMineral(&mineral) {}

	bool Mineral::exists() {
		return bwemMineral;
	}

	void Mineral::addWorker(Worker& worker) {
		workers.push_back(worker);
	}

	Worker Mineral::removeWorker() {
		Worker worker = workers.back();
		workers.pop_back();
		return worker;
	}

	void Mineral::removeWorker(Worker& worker) {
		workers.erase(std::remove(workers.begin(), workers.end(), worker), workers.end());
	}
}