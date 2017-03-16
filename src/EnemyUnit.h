#pragma once
#include <BWAPI.h>

class EnemyUnit {
public:
	EnemyUnit();
	~EnemyUnit();

	int id;
	int areaId;
	BWAPI::UnitType type;
	BWAPI::Position position;
	BWAPI::TilePosition tilePosition;
	int lastSeen;
	bool isVisible;
};

