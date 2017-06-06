#include "UnitMix.h"
#include "BlackCrow.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	UnitMix::UnitMix(BlackCrow& blackCrow) : bc(blackCrow) {}

	void UnitMix::add(BWAPI::UnitType type, double weight) {
		unitWeights.emplace_back(std::make_pair(type, weight));
	}

	// Chooses next unit based on past picks and probability
	BWAPI::UnitType UnitMix::getAndMarkNextUnit() {
		return unitWeights.front().first; // TODO
	}
}