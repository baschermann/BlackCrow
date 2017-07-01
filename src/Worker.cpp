#include "Worker.h"
#include "Mineral.h"
#include "Geyser.h"
#include "Base.h"

///////////////////////////////////////////////////////////
//				Worker mining notes                      //
///////////////////////////////////////////////////////////
//
// - Speed of Workers is exactly 5 not 4.920
// - Acceleration is 0.261719 or 67/256
// - Base size is important
// - Workers need to turn to mineral before mining (usually 1 frame)
// - The mining animation is "AlmostBuilt" (makes total sense)
// - Mining timer is 75 frames
//   - Can only be checked every 8 frames that runs on a global timer
//   - Global timer is being reset every 150 frames to a random number
//     - Iterates through all the units and resets the timer
//	   - This can cause a bad pattern where it takes 6 [150 mod 8 = 4] + 8 frames to check
//     - Random unit timer is wrapped at 8
//
/////////////////////////////////////////////////////////////


namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Worker::Worker(BWAPI::Unit worker, Base& base) : unit(worker), base(&base) {}

	void Worker::onFrame() {
		if (miningTarget == MiningTarget::MINERAL) {
			Unit orderTarget = unit->getOrderTarget();

			if (orderTarget && orderTarget->getType().isRefinery()) {
				continueMining();
			}
		}

		resetAfterAttackFrameCounter--;
		if (resetAfterAttackFrameCounter == 0)
			continueMining();
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
		geyser = nullptr;
	}

	void Worker::setGeyser(Geyser& pGeyser) {
		miningTarget = MiningTarget::GEYSER;
		geyser = &pGeyser;
		mineral = nullptr;
	}

	void Worker::reset() {
		mineral = nullptr;
		geyser = nullptr;
	}

	bool operator==(const Worker& left, const Worker& right) {
		return left.unit->getID() == right.unit->getID();
	}
}