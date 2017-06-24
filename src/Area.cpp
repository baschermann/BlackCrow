#include "Area.h"
#include <algorithm>

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;


	Area::Area(BlackCrow &parent, const BWEM::Area& bwemArea) : bc(parent), bwemArea(bwemArea), isEnemyOwned(false) {}

	void Area::addEnemyBuilding(EnemyUnitPtr enemyBuilding) {
		enemyBuildings.push_back(enemyBuilding);
	}

	void Area::removeEnemyBuilding(EnemyUnitPtr enemyBuilding) {
		enemyBuildings.erase(std::remove(enemyBuildings.begin(), enemyBuildings.end(), enemyBuilding), enemyBuildings.end());
	}
}