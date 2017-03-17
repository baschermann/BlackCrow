#include "Area.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;


	Area::Area(BlackCrow &parent) : bc(parent) {
		occupied = false;
		enemyBuildings = new std::list<EnemyUnit*>();
	}


	Area::~Area() {
	}

	void Area::addEnemyBuilding(EnemyUnit* enemyBuilding) {
		enemyBuildings->push_back(enemyBuilding);
	}

	void Area::removeEnemyBuilding(EnemyUnit* enemyBuilding) {
		enemyBuildings->remove(enemyBuilding);
	}

	bool Area::isOccupied() {
		return occupied;
	}
}