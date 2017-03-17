#include "Enemy.h"

using namespace BWAPI;
using namespace Filter;
//namespace { BWEM::Map* map; }

Enemy* enemy = new Enemy();

Enemy::Enemy() {
	enemies = new std::list <EnemyUnit*>();
	
}


Enemy::~Enemy() {
}

void Enemy::onStart() {
	//map = &BWEM::Map::Instance();
}

void Enemy::enemyDiscovered(BWAPI::Unit unit) {
	EnemyUnit* enemy = findEnemy(unit->getID());

	if (!enemy) {
		enemy = new EnemyUnit();
		enemy->id = unit->getID();
		enemy->isVisible = true;
		enemy->lastSeen = Broodwar->getFrameCount();
		enemy->type = unit->getType();
		enemy->tilePosition.x = -1;
		enemy->tilePosition.y = -1;

		enemies->push_front(enemy);
	}

	if (enemy->type.isBuilding()) {
		if (enemy->tilePosition.x != unit->getTilePosition().x || enemy->tilePosition.y != unit->getTilePosition().y) {
			enemy->tilePosition = unit->getTilePosition();

			assert(enemy->tilePosition.x != 1);
			assert(enemy->tilePosition.y != 1);

			const BWEM::Area* a = BWEM::Map::Instance().GetNearestArea(enemy->tilePosition);
			enemy->areaId = a->Id();
		}
	} else {
		enemy->position = unit->getPosition();
	}
}

void Enemy::onFrame() {
	for (EnemyUnit* eu : *enemies) {
		Unit unit = Broodwar->getUnit(eu->id);
		bool a = unit->isVisible();

		if (unit->isVisible()) {
			if (eu->type.isBuilding() && (unit->getTilePosition().x != eu->tilePosition.x || unit->getTilePosition().y != eu->tilePosition.y)) {
				eu->tilePosition.x = unit->getTilePosition().x;
				eu->tilePosition.y = unit->getTilePosition().y;
			} else {
				eu->position.x = unit->getPosition().x;
				eu->position.y = unit->getPosition().y;
			}
		}
	}
}

void Enemy::enemyDestroyed(BWAPI::Unit unit) {

}

EnemyUnit* Enemy::findEnemy(int id) {
	for (EnemyUnit* enemyUnit : *enemies) {
		if (enemyUnit->id == id)
			return enemyUnit;
	}
	return nullptr;
}