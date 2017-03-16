#pragma once
#include <BWAPI.h>
#include <BWEM/bwem.h> 
#include "EnemyUnit.h" 

class Enemy {
public:
	Enemy();
	~Enemy();

	void onStart();
	void onFrame();
	std::list<EnemyUnit*>* enemies;
	void enemyDiscovered(BWAPI::Unit unit);
	void enemyDestroyed(BWAPI::Unit unit);

private:
	EnemyUnit* findEnemy(int id);
};

extern Enemy* enemy;