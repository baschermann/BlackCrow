#include "Squad.h"

using namespace BWAPI;
using namespace Filter;
namespace { BWEM::Map& map = BWEM::Map::Instance(); }

void Squad::init() {
	
}

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
ScoutSquad::ScoutSquad() {
	scoutLocations = new std::list<BWAPI::TilePosition>();
}

ScoutSquad::~ScoutSquad() {
	delete scoutLocations;
}

void ScoutSquad::init() {
	Squad::init();
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
	for (BWAPI::TilePosition tilePosition : map.StartingLocations()) {
		if(!Broodwar->isVisible(tilePosition)) {
			tilePosition.x += 1; // TODO Do I change the starting position?!?
			tilePosition.y += 1;
			addScoutPosition(tilePosition);
		}
	}
}

void ScoutSquad::addExpansions() {
	for (BWEM::Area area : map.Areas()) {
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