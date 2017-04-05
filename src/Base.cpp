#include "BlackCrow.h"
#include "Base.h"
#include "Area.h"
#include "Worker.h"


namespace BlackCrow {

	
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
		// Remove empty minerals and put the drones on another mineral
		for (Mineral& mineral : minerals) {
			if (!mineral.exists()) {
				std::vector<Worker> workers = mineral.workers;
				minerals.erase(std::remove(minerals.begin(), minerals.end(), mineral), minerals.end());
				for (Worker& worker : workers) {
					worker.removeFromResource();
					worker.setMineral(findMineralForWorker());
				}
			}
		}
	}

	bool Base::isEstablished() {
		if (hatchery && hatchery->exists() && hatchery->isCompleted())
			return true;
		return false;
	}

	void Base::addWorker(BWAPI::Unit unit) {
		Worker cWorker(unit, *this);
		cWorker.setMineral(findMineralForWorker());
	}

	// Can return nullptr
	BWAPI::Unit Base::removeWorker() {
		int highestWorkerCount = 0;
		for (Mineral& mineral : minerals) {
			highestWorkerCount = std::max(highestWorkerCount, (int) mineral.workers.size());
		}

		for (Mineral& mineral : minerals) {
			if (mineral.workers.size() == highestWorkerCount) {
				Worker worker = mineral.workers.back();
				worker.removeFromResource();
				return worker.unit;
			}
		}
		
		// TODO Take from Gas
		return nullptr;
	}

	// Can return nullptr
	BWAPI::Unit Base::removeWorker(BWAPI::Position closestTo) {
		double minDistance = std::numeric_limits < double >::max() ;
		Worker* minWorker = nullptr;


		std::vector<Worker> mineralWorkers = getMineralWorkers();
		if (mineralWorkers.size() > 0) {

			for (Worker& worker : mineralWorkers) {
				double distance = Util::distance(closestTo, worker.unit->getPosition());
				if (!minWorker) {
					minWorker = &worker;
					minDistance = distance;
				} else if (distance < minDistance) {
					minWorker = &worker;
					minDistance = distance;
				}
			}

			Worker worker = *minWorker;
			worker.removeFromResource();
			return worker.unit;
		}

		// TODO Take from Gas
		return nullptr;
	}

	bool Base::workerNeeded() {
		if (hatchery && hatchery->exists() && hatchery->isCompleted())
			return totalMineralWorkers() < bc.config.mineralSaturationMultiplier * minerals.size() || totalGasWorkers() < (int)(geysers.size() * 3);
		else
			return false;
	}

	int Base::workersNeeded() {
		if (hatchery && hatchery->exists() && hatchery->isCompleted())
			return (int)(bc.config.mineralSaturationMultiplier * minerals.size() - totalMineralWorkers() + geysers.size() * 3 - totalGasWorkers());
		else
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

	int Base::totalWorkers() {
		return totalMineralWorkers() + totalGasWorkers();
	}

	int Base::totalMineralWorkers() {
		int amount = 0;
		for (Mineral& mineral : minerals) {
			amount += mineral.workers.size();
		}
		return amount;
	}

	int Base::totalGasWorkers() {
		int amount = 0;
		for (Geyser& geyser : geysers) {
			amount += geyser.workers.size();
		}
		return amount;
	}

	std::vector<Worker> Base::getMineralWorkers() {
		std::vector<Worker> mineralWorkers;
		for (Mineral& mineral : minerals) {
			mineralWorkers.insert(mineralWorkers.end(), mineral.workers.begin(), mineral.workers.end());
		}
		return mineralWorkers;
	}

	std::vector<Worker> Base::getGasWorkers() {
		std::vector<Worker> gasWorkers;
		for (Geyser& geyser : geysers) {
			gasWorkers.insert(gasWorkers.end(), geyser.workers.begin(), geyser.workers.end());
		}
		return gasWorkers;
	}
}