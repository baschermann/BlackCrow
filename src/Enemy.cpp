#include "Enemy.h"
#include "EnemyUnit.h"
#include <assert.h>
#include <BWEM/bwem.h>
#include "BlackCrow.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Enemy::Enemy(BlackCrow &parent) : bc(parent) {
	}

	void Enemy::onStart() {
	}

	void Enemy::enemyDiscovered(BWAPI::Unit unit) {
		if (Broodwar->self()->isEnemy(unit->getPlayer())) {
			EnemyUnit* enemy = findEnemy(unit->getID());

			if (!enemy) {
				enemies.emplace_back();
				EnemyUnit& newEnemy = enemies.back();

				newEnemy.id = unit->getID();
				newEnemy.isVisible = true;
				newEnemy.lastSeen = Broodwar->getFrameCount();
				newEnemy.type = unit->getType();
				newEnemy.tilePosition = BWAPI::TilePositions::Invalid;

				enemy = &newEnemy;
			}

			if (enemy->type.isBuilding()) {
				if (enemy->tilePosition.x != unit->getTilePosition().x || enemy->tilePosition.y != unit->getTilePosition().y) {
					enemy->tilePosition = unit->getTilePosition();

					assert(enemy->tilePosition != BWAPI::TilePositions::Invalid);

					const BWEM::Area* area = bc.bwem.GetNearestArea(enemy->tilePosition);
					enemy->areaId = area->Id();
				}
			} else {
				enemy->position = unit->getPosition();
			}
		}
	}

	void Enemy::onFrame() {
		for (EnemyUnit eu : enemies) {
			Unit unit = Broodwar->getUnit(eu.id);

			if (unit->isVisible()) {
				if (eu.type.isBuilding() && (unit->getTilePosition().x != eu.tilePosition.x || unit->getTilePosition().y != eu.tilePosition.y)) {
					eu.tilePosition.x = unit->getTilePosition().x;
					eu.tilePosition.y = unit->getTilePosition().y;
				} else {
					eu.position.x = unit->getPosition().x;
					eu.position.y = unit->getPosition().y;
				}
			}
		}
	}

	void Enemy::enemyDestroyed(BWAPI::Unit unit) {

	}

	EnemyUnit* Enemy::findEnemy(int id) {
		for (EnemyUnit& enemyUnit : enemies) {
			if (enemyUnit.id == id)
				return &enemyUnit;
		}
		return nullptr;
	}
}