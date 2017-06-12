#include "UnitMix.h"
#include "BlackCrow.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	UnitMix::UnitMix(BlackCrow& blackCrow) : bc(blackCrow) {}

	void UnitMix::set(BWAPI::UnitType type, double weight) {
		auto propertyIt = std::find_if(propertyUnits.begin(), propertyUnits.end(), [type](SpawnProperty& property) { return property.type == type; });
		if (propertyIt == propertyUnits.end())
			propertyUnits.emplace_back(type, weight, weight);
		else
			propertyIt->weight = weight;

		calculate();
		resetSpawnValues();
		chooseNext();
	}

	void UnitMix::remove(BWAPI::UnitType type) {
		propertyUnits.erase(std::remove_if(propertyUnits.begin(), propertyUnits.end(), [type](SpawnProperty& property) { return property.type == type; }));

		calculate();
		//resetSpawnValues();
		if (next == type)
			chooseNext();
	}

	double UnitMix::get(BWAPI::UnitType type) {
		auto propertyIt = std::find_if(propertyUnits.begin(), propertyUnits.end(), [type](SpawnProperty& property) { return property.type == type; });
		if (propertyIt != propertyUnits.end())
			return propertyIt->weight;
		else
			return std::numeric_limits<double>::max();
	}

	bool UnitMix::exists(BWAPI::UnitType type) {
		if (get(type) != std::numeric_limits<double>::max())
			return true;
		return false;
	}
	
	BWAPI::UnitType UnitMix::peek() {
		return next;
	}

	BWAPI::UnitType UnitMix::pop() {
		BWAPI::UnitType type = next;
		chooseNext();
		return type;
	}

	double UnitMix::larvaPerFrame() {
		return larvaFrame;
	}

	double UnitMix::mineralPerFrame() {
		return mineralFrame;
	}

	double UnitMix::gasPerFrame() {
		return gasFrame;
	}

	double UnitMix::supplyPerFrame() {
		return supplyFrame;
	}

	int UnitMix::size() {
		return propertyUnits.size();
	}

	void UnitMix::calculate() {
		// Total Weight
		totalWeight = 0;

		for (SpawnProperty& property : propertyUnits) {
			totalWeight += property.weight;
		}

		// other
		larvaFrame = 0;
		mineralFrame = 0;
		gasFrame = 0;

		for (SpawnProperty& property : propertyUnits) {
			double comparedWeight = property.weight / totalWeight;

			int buildTime = property.type.buildTime();
			larvaFrame += (1.0 / (double)property.type.buildTime()) * comparedWeight;
			mineralFrame += ((double)property.type.mineralPrice() / (double)property.type.buildTime()) * comparedWeight;
			gasFrame += ((double)property.type.gasPrice() / (double)property.type.buildTime()) * comparedWeight;
			supplyFrame += ((double)property.type.supplyRequired() / (double)property.type.buildTime()) * comparedWeight;
		}
	}

	// Chooses next unit based on past picks and probability
	void UnitMix::chooseNext() {
		if (propertyUnits.size() > 0) {
			double weightAmount = totalWeight / propertyUnits.size();
			std::set<BWAPI::UnitType> randomBucket;

			for (SpawnProperty& property : propertyUnits) {
				double spawnValue = property.spawnValue;
				while (spawnValue > 0) {
					randomBucket.insert(property.type);
					spawnValue -= weightAmount;
				}
			}

			if (randomBucket.size() <= 0) {
				for (SpawnProperty& property : propertyUnits)
					property.spawnValue += property.weight;
				chooseNext();
				return;
			}

			int random = Util::randomIntIncl(0, randomBucket.size() - 1);
			UnitType chosenUnitType = *std::next(randomBucket.begin(), random);

			auto propertyIt = std::find_if(propertyUnits.begin(), propertyUnits.end(), [chosenUnitType](SpawnProperty& property) { return property.type == chosenUnitType; });
			if (propertyIt != propertyUnits.end()) {
				propertyIt->spawnValue -= weightAmount;
			}

			next = chosenUnitType;
			return;
		}
		next = UnitTypes::None;
	}

	void UnitMix::resetSpawnValues() {
		for (SpawnProperty& property : propertyUnits)
			property.spawnValue = property.weight;
	}
}