#include "AreaData.h"

using namespace BWAPI;
using namespace Filter;
//namespace { BWEM::Map* map = &BWEM::Map::Instance(); }


AreaData::AreaData() {
	occupied = false;
	enemyBuildings = new std::list<EnemyUnit*>();
}


AreaData::~AreaData() {
}

void AreaData::addEnemyBuilding(EnemyUnit* enemyBuilding) {
	enemyBuildings->push_back(enemyBuilding);
}

void AreaData::removeEnemyBuilding(EnemyUnit* enemyBuilding) {
	enemyBuildings->remove(enemyBuilding);
}

bool AreaData::isOccupied() {
	return occupied;
}