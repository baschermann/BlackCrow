#define NOMINMAX
#include "BlackCrow.h"
#include "Base.h"
#include "Area.h"
#include "Worker.h"
#include <numeric>
#include <algorithm>
#include <Filter.h>

namespace BlackCrow {

	static int a = 0;

	Base::Base(BlackCrow& parent, const BWEM::Base& bwemBase, Area& area) : bc(parent), bwemBase(bwemBase), area(area) {
		// Minerals
		for (const BWEM::Mineral* bwemMineral : bwemBase.Minerals()) {
			minerals.emplace_back(bwemMineral);
		}

		// Geysers
		for (const BWEM::Geyser* bwemGeyser : bwemBase.Geysers()) {
			geysers.emplace_back(bwemGeyser);
		}

		// Is Island
		//TODO
	}

	void Base::onFrame() {
		for (Worker& worker : workers) {
			if (worker.mineral && worker.mineral->id <= 0)
				assert(!"Wrong mineral ids!");
			worker.onFrame();
		}
	}

	bool Base::onUnitDestroyed(BWAPI::Unit unit) {
		if (unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field) {
			auto it = std::find_if(minerals.begin(), minerals.end(), [&](Mineral& mineral) { return mineral.bwemMineral->Unit() == unit; });
			if (it != minerals.end() && it->workers.size() > 0) {
				for (Worker* worker : it->workers) {
					Mineral& mineral = findMineralForWorker();
					worker->setMineral(mineral);
					mineral.registerWorker(*worker);
				}
				minerals.erase(it);
				return true;
			}
		}
		return false;
	}

	const bool Base::isEstablished() {
		if (hatchery && hatchery->exists() && hatchery->isCompleted())
			return true;
		return false;
	}

	void Base::addWorker(BWAPI::Unit unit) {
		//In c++17, emplace_back returns the created element.
		workers.emplace_back(unit, *this);

		Worker& worker = workers.back();
		Mineral& mineral = findMineralForWorker();

		worker.setMineral(mineral);
		mineral.registerWorker(worker);
		worker.continueMining();
	}

	// Can return nullptr
	BWAPI::Unit Base::removeWorker() {
		int highestWorkerCount = std::accumulate(minerals.begin(), minerals.end(), 0, [](int highest, Mineral& mineral) { return highest = std::max(highest, (int)mineral.workers.size()); });

		if (highestWorkerCount > 0) {
			for (Mineral& mineral : minerals) {
				if (mineral.workers.size() == highestWorkerCount) {
					Worker& worker = *mineral.workers.back();

					mineral.unregisterWorker(worker);
					workers.erase(std::remove(workers.begin(), workers.end(), worker), workers.end());

					return worker.unit;
				}
			}
		}
		// TODO Take from Gas
		return nullptr;
	}

	// Can return nullptr
	BWAPI::Unit Base::removeWorker(BWAPI::Position closestTo) {
		// TODO this is calculating to much DISTANCE!!
		auto worker = std::min_element(workers.begin(), workers.end(), [&](Worker& left, Worker& right) { return Util::distance(left.unit->getPosition(), closestTo) < Util::distance(right.unit->getPosition(), closestTo); });

		if (worker != workers.end()) {
			BWAPI::Unit unit = worker->unit;
			worker->mineral->unregisterWorker(*worker);
			workers.erase(worker);
			return unit;
		} else 
			return nullptr;
	}

	const bool Base::workerNeeded() {
		if (hatchery && hatchery->exists() && hatchery->isCompleted()) {
			if (totalMineralWorkers() < bc.config.mineralSaturationMultiplier * minerals.size())
				return true;

			for (Geyser& geyser : geysers) {
				if (geyser.workerNeeded())
					return true;
			}
		}
		return false;
	}

	const int Base::workersNeeded() {
		if (hatchery && hatchery->exists() && hatchery->isCompleted()) {
			return (int)(bc.config.mineralSaturationMultiplier * minerals.size() - totalMineralWorkers())
				+ std::accumulate(geysers.begin(), geysers.end(), 0, [](int sum, Geyser& geyser) { return sum += geyser.workersNeeded(); });
		}
		return 0;
	}

	Mineral& Base::findMineralForWorker() {
		for (unsigned int maxWorkers = 0;; maxWorkers++) {
			for (Mineral& mineral : minerals) {
				if (mineral.workers.size() <= maxWorkers)
					return mineral;
			}
		}
	}

	const int Base::totalWorkers() {
		return totalMineralWorkers() + totalGasWorkers();
	}

	const int Base::totalMineralWorkers() {
		return std::accumulate(workers.begin(), workers.end(), 0, [](int sum, Worker& worker) { return worker.miningTarget == Worker::MiningTarget::MINERAL ? ++sum : sum; });
	}

	const int Base::totalGasWorkers() {
		return std::accumulate(workers.begin(), workers.end(), 0, [](int sum, Worker& worker) { return worker.miningTarget == Worker::MiningTarget::GEYSER ? ++sum : sum; });
	}
}