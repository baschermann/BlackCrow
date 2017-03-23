#include "Worker.h"
#include "Mineral.h"

namespace BlackCrow {

	Worker::Worker(BWAPI::Unit worker, Base& base) : unit(worker), base(base) {}

	void Worker::stopMining() {
		unit->stop();
	}

	bool Worker::continueMining() {
		switch (miningTarget) {
		case MiningTarget::MINERAL:
			assert(mineral);

			if (mineral->exists()) {
				unit->gather(mineral->bwemMineral->Unit());
				return true;
			}

			break;
		case MiningTarget::GEYSER:
			assert(geyser);

			if (geyser->isMineable()) {
				unit->gather(geyser->geyserUnit);
				return true;
			}

			break;
		}

		return false;
	}

	bool Worker::setMineral(Mineral& mineral) {
		miningTarget = MiningTarget::MINERAL;
		return continueMining();
	}

	bool Worker::setGeyser(Geyser& geyser) {
		miningTarget = MiningTarget::GEYSER;
		return continueMining();
	}

	void Worker::removeFromResource() {
		stopMining();

		switch (miningTarget) {
		case MiningTarget::MINERAL:
			assert(mineral);

			mineral->removeWorker(*this);

			break;
		case MiningTarget::GEYSER:
			assert(geyser);

			geyser->removeWorker(*this);

			break;
		}
	}

	bool operator==(const Worker& left, const Worker& right) {
		return left.unit->getID() == right.unit->getID();
	}
}