#pragma once
#include <BWAPI.h>
#include <utility>

namespace BlackCrow {

	class BlackCrow;

	class UnitMix {
	public:
		UnitMix(BlackCrow& blackCrow);

		void add(BWAPI::UnitType type, double weight);
		int size();
		BWAPI::UnitType getAndMarkNextUnit();

	private:
		BlackCrow& bc; 
		std::vector<std::pair<BWAPI::UnitType, double>> unitWeights;
	};

}