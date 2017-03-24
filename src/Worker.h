#pragma once
#include <BWAPI.h>
#include "Mineral.h"
#include "Geyser.h"
#include "Base.h"

namespace BlackCrow {

	class Mineral;
	class Geyser;

	class Worker {
	public:
		Worker(BWAPI::Unit worker, Base& base);

		enum class MiningTarget {
			MINERAL,
			GEYSER,
			NONE
		};

		BWAPI::Unit unit;
		std::reference_wrapper<Base> base;
		MiningTarget miningTarget = MiningTarget::NONE;
		Mineral* mineral = nullptr;
		Geyser* geyser = nullptr;

		void stopMining();
		bool continueMining();
		bool setMineral(Mineral& mineral);
		bool setGeyser(Geyser& geyser);
		void removeFromResource();
	};

	bool operator==(const Worker& left, const Worker& right);
}