#pragma once
#include <BWAPI.h>
#include <tuple>

namespace BlackCrow {

	class BlackCrow;

	class UnitMix {
	public:

		

		UnitMix(BlackCrow& blackCrow);

		void add(BWAPI::UnitType type, double weight);
		int size();
		BWAPI::UnitType peek();
		BWAPI::UnitType pop();

		double larvaPerFrame();
		double mineralPerFrame();
		double gasPerFrame();

	private:
		struct SpawnProperty {
			BWAPI::UnitType type;
			double weight;
			double spawnValue;

			SpawnProperty(BWAPI::UnitType type, double weight, double spawnValue) : type(type), weight(weight), spawnValue(spawnValue) {}
		};

		BlackCrow& bc; 
		std::vector<SpawnProperty> propertyUnits;
		BWAPI::UnitType next = BWAPI::UnitTypes::None;

		double totalWeight = 0;
		double larvaFrame = 0;
		double mineralFrame = 0;
		double gasFrame = 0;

		void calculate();
		void chooseNext();
		void resetSpawnValues();
	};

}