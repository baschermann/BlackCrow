#pragma once
#include <BWAPI.h>
#include <memory>
#include "SquadUnit.h"
#include "Common.h"

// Handles single units in a squad. Micro, target decision (focusfire) and positioning

namespace BlackCrow {

	class BlackCrow;
	class SquadUnit;

	class Squad {
	public:

		Squad();
		void onFrame();

		std::vector<SquadUnitPtr> sunits;

		void add(const SquadUnitPtr sunit);
		void remove(const SquadUnitPtr sunit);
		void moveAll(BWAPI::Position position, bool queue);


	protected:
		BlackCrow &bc;
		Squad(BlackCrow &parent);
	};

	class ScoutSquad : public Squad {
	public:

		ScoutSquad(BlackCrow &parent);
		void onFrame();
		bool isStillScouting();
		int locationsToScout();
		std::vector<BWAPI::TilePosition>& getScoutingPositions();
		void addScoutPosition(BWAPI::TilePosition position);
		void addStartLocations();
		void addExpansions();
		void setGlobalSearch(bool searchGlobally);

	private:
		bool globalSearch;
		void ScoutSquad::disband();
		std::vector<BWAPI::TilePosition> scoutLocations;
	};

	class AttackSquad : public Squad {
	public:

		enum class State {
			MOVE,
			ATTACK
		};

		// Common Functions
		AttackSquad(BlackCrow &parent);
		void onFrame();

		// Variables
		State state = State::MOVE;

		// Functions
		

	};

	class StallSquad : public Squad {
	public:
	};
}