#pragma once
#include <BWAPI.h>

namespace BlackCrow {

	class Base;
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
		Base* base;
		MiningTarget miningTarget = MiningTarget::NONE;
		Mineral* mineral = nullptr;
		Geyser* geyser = nullptr;

		void onFrame();
		void stopMining();
		bool continueMining();
		void setMineral(Mineral& mineral);
		void setGeyser(Geyser& geyser);
		void reset();
	};

	bool operator==(const Worker& left, const Worker& right);
}