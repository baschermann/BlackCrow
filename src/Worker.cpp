#include "Worker.h"
#include "Mineral.h"
#include "Geyser.h"
#include "Base.h"

namespace BlackCrow {

	Worker::Worker(BWAPI::Unit worker, Base& base) : unit(worker), base(&base) {}

	void Worker::onFrame() {

	}

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

	void Worker::setMineral(Mineral& pMineral) {
		miningTarget = MiningTarget::MINERAL;
		mineral = &pMineral;
	}

	void Worker::setGeyser(Geyser& pGeyser) {
		miningTarget = MiningTarget::GEYSER;
		geyser = &pGeyser;
	}

	bool operator==(const Worker& left, const Worker& right) {
		return left.unit->getID() == right.unit->getID();
	}
}