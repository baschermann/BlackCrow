#pragma once
#include <BWAPI.h>
#include <tuple>

namespace BlackCrow {

	class BlackCrow;

	class UnitMix {
	public:
		UnitMix(BlackCrow& blackCrow);

		void set(BWAPI::UnitType type, double weight, bool reset);
		void remove(BWAPI::UnitType type);
		double get(BWAPI::UnitType type);
		bool exists(BWAPI::UnitType type);
		int size();
		BWAPI::UnitType peek();
		BWAPI::UnitType pop();

		double larvaPerFrame();
		double mineralPerFrame();
		double gasPerFrame();
		double supplyPerFrame();

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
		double supplyFrame = 0;

		void calculate();
		void chooseNext();
		void resetSpawnValues();
	};

}