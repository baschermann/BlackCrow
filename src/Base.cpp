#define NOMINMAX
#include "BlackCrow.h"
#include "Base.h"
#include "Area.h"
#include "Worker.h"
#include <numeric>
#include <algorithm>
#include <Filter.h>
#include "Common.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

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
		for (WorkerPtr worker : workers) {
			worker->onFrame();
		}
	}

	bool Base::onUnitDestroyed(BWAPI::Unit unit) {
		if (unit->getType().isMineralField()) {
			auto it = std::find_if(minerals.begin(), minerals.end(), [&](Mineral& mineral) { return mineral.bwemMineral->Unit() == unit; });
			if (it != minerals.end() && it->workers.size() > 0) {

				for (WorkerPtr worker : it->workers) {
					Mineral* mineral = findMineralForWorker();
					worker->mineral->unregisterWorker(worker);

					if (mineral) {
						worker->setMineral(*mineral);
						mineral->registerWorker(worker);
					}
				}
				minerals.erase(it);
				return true;
			}
		}

		if (unit->getType() == UnitTypes::Zerg_Drone) {
			auto workerIt = std::find_if(workers.begin(), workers.end(), [&](WorkerPtr worker) { return worker->unit->getID() == unit->getID(); });
			if (workerIt != workers.end()) {

				WorkerPtr worker = *workerIt;
				if (worker->miningTarget == Worker::MiningTarget::MINERAL) {
					worker->mineral->unregisterWorker(worker);
				} else if (worker->miningTarget == Worker::MiningTarget::GEYSER) {
					worker->geyser->unregisterWorker(worker);
				}

				workers.erase(workerIt);
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
		//workers.emplace_back(unit, *this);

		WorkerPtr worker = std::make_shared<Worker>(unit, *this);
		workers.push_back(worker);
		
		Mineral* mineral = findMineralForWorker();
		if(mineral) {
			worker->setMineral(*mineral);
			mineral->registerWorker(worker);
			worker->continueMining();
		}
	}

	// Can return nullptr
	BWAPI::Unit Base::removeWorker() {
		int highestWorkerCount = std::accumulate(minerals.begin(), minerals.end(), 0, [](int highest, Mineral& mineral) { return highest = std::max(highest, (int)mineral.workers.size()); });

		if (highestWorkerCount > 0) {
			for (Mineral& mineral : minerals) {
				if (mineral.workers.size() == highestWorkerCount) {
					WorkerPtr worker = mineral.workers.back();

					mineral.unregisterWorker(worker);
					workers.erase(std::remove(workers.begin(), workers.end(), worker), workers.end());
					worker->reset();

					return worker->unit;
				}
			}
		}
		// TODO Take from Gas
		return nullptr;
	}

	// Can return nullptr
	BWAPI::Unit Base::removeWorker(BWAPI::Position closestTo) {
		// TODO this is calculating to much DISTANCE!!
		auto workerIt = std::min_element(workers.begin(), workers.end(), [&](WorkerPtr left, WorkerPtr right) { return Util::distance(left->unit->getPosition(), closestTo) < Util::distance(right->unit->getPosition(), closestTo); });
		if (workerIt != workers.end()) {

			WorkerPtr worker = *workerIt;
			if (worker->mineral)
				worker->mineral->unregisterWorker(worker);
			if (worker->geyser)
				worker->geyser->unregisterWorker(worker);

			workers.erase(workerIt);
			worker->reset();

			return worker->unit;
		} else 
			return nullptr;
	}

	const bool Base::isWorkerNeeded() {
		if (hatchery && hatchery->exists() && hatchery->isCompleted()) {
			if (getTotalMineralWorkers() < bc.config.mineralSaturationMultiplier * minerals.size())
				return true;

			for (Geyser& geyser : geysers) {
				if (geyser.workerNeeded())
					return true;
			}
		}
		return false;
	}

	const int Base::getWorkersNeeded() {
		if (hatchery && hatchery->exists() && hatchery->isCompleted()) {
			return (int)(bc.config.mineralSaturationMultiplier * minerals.size() - getTotalMineralWorkers())
				+ std::accumulate(geysers.begin(), geysers.end(), 0, [](int sum, Geyser& geyser) { return sum += geyser.isMineable() ? geyser.workersNeeded() : 0; });
		}
		return 0;
	}

	bool Base::shiftWorkerToGas() {
		
		auto geyser = std::find_if(geysers.begin(), geysers.end(), [](Geyser& geyser) { return geyser.workerNeeded(); });
		if (geyser != geysers.end()) {
			BWAPI::Position closestTo = BWAPI::Position(geyser->bwemGeyser->TopLeft());

			// TODO Get the closest drone instead of a random one
			// TODO this is calculating to much DISTANCE!!
			//auto worker = std::min_element(workers.begin(), workers.end(), [&](Worker& left, Worker& right) { return Util::distance(left.unit->getPosition(), closestTo) < Util::distance(right.unit->getPosition(), closestTo); });
			auto workerIt = std::find_if(workers.begin(), workers.end(), [](WorkerPtr worker) { return worker->miningTarget == Worker::MiningTarget::MINERAL; });
			if (workerIt != workers.end()) {

				WorkerPtr worker = *workerIt;
				worker->mineral->unregisterWorker(worker);
				worker->reset();

				worker->setGeyser(*geyser);
				geyser->registerWorker(worker);
				worker->continueMining();
				return true;
			}
			Broodwar->sendText("No worker could be shifted from minerals to gas. This should not happen");
		}
		Broodwar->sendText("Could not find geyser that needs worker for the chosen base. This should not happen");
		return false;
	}

	void Base::shiftWorkerFromGas() {
		auto workerIt = std::find_if(workers.begin(), workers.end(), [](WorkerPtr worker) { return worker->miningTarget == Worker::MiningTarget::GEYSER; });
		if (workerIt != workers.end()) {

			WorkerPtr worker = *workerIt;
			worker->geyser->unregisterWorker(worker);
			worker->reset();

			Mineral* mineral = findMineralForWorker();
			if (mineral) {
				worker->setMineral(*mineral);
				mineral->registerWorker(worker);
			}
			worker->continueMining();
			return;
		}
		Broodwar->sendText("No worker could be shifted from gas to minerals. This should not happen");
	}

	Mineral* Base::findMineralForWorker() {
		for (unsigned int maxWorkers = 0;; maxWorkers++) {
			for (Mineral& mineral : minerals) {
				if (mineral.workers.size() <= maxWorkers && mineral.exists())
					return &mineral;
			}
		}
		return nullptr;
	}

	const int Base::getTotalWorkers() {
		return getTotalMineralWorkers() + getTotalGasWorkers();
	}

	const int Base::getTotalMineralWorkers() {
		return std::accumulate(workers.begin(), workers.end(), 0, [](int sum, WorkerPtr worker) { return worker->miningTarget == Worker::MiningTarget::MINERAL ? ++sum : sum; });
	}

	const int Base::getTotalGasWorkers() {
		return std::accumulate(workers.begin(), workers.end(), 0, [](int sum, WorkerPtr worker) { return worker->miningTarget == Worker::MiningTarget::GEYSER ? ++sum : sum; });
	}

	const int Base::getGasWorkerSlotsAvailable() {
		return std::accumulate(geysers.begin(), geysers.end(), 0, [](int sum, Geyser& geyser) { return geyser.isMineable() ? ++sum : sum; }) * 3 - getTotalGasWorkers();
	}

	const int Base::getExtractorsAbleToBuild() {
		return std::accumulate(geysers.begin(), geysers.end(), 0, [](int sum, Geyser& geyser) { return geyser.isBuildable() ? ++sum : sum; });
	}
}