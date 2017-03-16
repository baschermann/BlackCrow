#pragma once
#include <BWAPI.h>
#include <BWEM/bwem.h>
#include "EnemyUnit.h"

class AreaData {
public:
	AreaData();
	~AreaData();

	
	const BWEM::Area* area;
	void addEnemyBuilding(EnemyUnit* enemyBuilding);
	void removeEnemyBuilding(EnemyUnit* enemyBuilding);
	bool isOccupied();

private:
	bool occupied;
	std::list<EnemyUnit*>* enemyBuildings;

};

