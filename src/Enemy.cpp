#include "Enemy.h"
#include "EnemyUnit.h"
#include <assert.h>
#include <BWEM/bwem.h>
#include "BlackCrow.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Enemy::Enemy(BlackCrow &parent) : bc(parent) {}

	void Enemy::onStart() {}

	void Enemy::enemyDiscovered(BWAPI::Unit unit) {
		if (Broodwar->self()->isEnemy(unit->getPlayer())) {
			EnemyUnit* enemy = getEnemy(unit->getID());

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
		for (EnemyUnit& eu : enemies) {
			Unit unit = Broodwar->getUnit(eu.id);

			if (unit->isVisible()) {
				eu.isGhost = false;

				if (eu.type.isBuilding() && (unit->getTilePosition().x != eu.tilePosition.x || unit->getTilePosition().y != eu.tilePosition.y)) {
					eu.tilePosition.x = unit->getTilePosition().x;
					eu.tilePosition.y = unit->getTilePosition().y;
				} else {
					eu.position.x = unit->getPosition().x;
					eu.position.y = unit->getPosition().y;
				}

				if (unit->getType() != eu.type) {
					eu.type = unit->getType();
				}

				eu.lastSeen = Broodwar->getFrameCount();	
			} else {
				eu.isVisible = false;

				// Handling a ghost
				if (Broodwar->isVisible(TilePosition(eu.position))) {
					eu.isGhost = true;
				} else {
					if (!eu.type.isBuilding() && eu.lastSeen + ghostTime(eu.type) <= Broodwar->getFrameCount()) {
						eu.isGhost = true;
					}
				}
			}
		}
	}

	void Enemy::onUnitDestroyed(BWAPI::Unit unit) {
		auto enemyUnitIt = enemies.begin();
		while (enemyUnitIt != enemies.end()) {
			if (enemyUnitIt->id == unit->getID()) {
				enemies.erase(enemyUnitIt);
				return;
			}
			else
				enemyUnitIt++;
		}
	}

	EnemyUnit* Enemy::getEnemy(int id) {
		for (EnemyUnit& enemyUnit : enemies) {
			if (enemyUnit.id == id)
				return &enemyUnit;
		}
		return nullptr;
	}

	double Enemy::ghostTime(BWAPI::UnitType type) {
		return 1700 / type.topSpeed();
	}
}