#include "Squad.h"
#include "SquadUnit.h"
#include "BWEM/bwem.h"
#include "BlackCrow.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Squad::Squad(BlackCrow &parent) : bc(parent) {}

	Squad::~Squad() {}

	void Squad::onFrame() {
	}

	void Squad::add(SquadUnit* unit) {
		units.insert(unit);
	}

	void Squad::remove(SquadUnit* unit) {
		units.erase(unit);
	}

	void Squad::moveAll(BWAPI::Position position, bool queue) {
		for (SquadUnit* unit : units) {
			unit->move(position, queue);
		}
	}

	// ######## Scout Squad #########
	ScoutSquad::ScoutSquad(BlackCrow &parent) : Squad(parent) {
		scoutLocations = new std::list<BWAPI::TilePosition>();
	}

	ScoutSquad::~ScoutSquad() {
		delete scoutLocations;
	}

	void ScoutSquad::onFrame() {
		Squad::onFrame();

		if (scoutLocations->size() > 0 && Broodwar->isVisible(scoutLocations->front())) {
			scoutLocations->pop_front();

			if (scoutLocations->size() > 0) {
				BWAPI::TilePosition tilePosition = scoutLocations->front();
				moveAll(BWAPI::Position(tilePosition.x * 32, tilePosition.y * 32), false);
			}
		}
	}

	bool ScoutSquad::isStillScouting() {
		return scoutLocations->size() > 0;
	}

	int ScoutSquad::locationsToScout() {
		return scoutLocations->size();
	}

	std::list<BWAPI::TilePosition>* ScoutSquad::getScoutingPositions() {
		return scoutLocations;
	}

	void ScoutSquad::addScoutPosition(BWAPI::TilePosition tilePosition) {
		if (scoutLocations->size() <= 0)
			moveAll(BWAPI::Position(tilePosition.x * 32, tilePosition.y * 32), false);

		scoutLocations->push_back(tilePosition);
	}

	void ScoutSquad::addStartLocations() {
		for (BWAPI::TilePosition tilePosition : bc.bwem.StartingLocations()) {
			if (!Broodwar->isVisible(tilePosition)) {
				tilePosition.x += 1; // TODO Do I change the starting position?!?
				tilePosition.y += 1;
				addScoutPosition(tilePosition);
			}
		}
	}

	void ScoutSquad::addExpansions() {
		for (BWEM::Area area : bc.bwem.Areas()) {
			for (BWEM::Base base : area.Bases()) {
				if (!Broodwar->isVisible(base.Location())) {
					addScoutPosition(base.Location());
				}
			}
		}
	}

	void ScoutSquad::setGlobalSearch(bool searchGlobally) {
		globalSearch = searchGlobally;
	}
}