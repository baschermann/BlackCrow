#pragma once
#include <memory>
#include <BWAPI.h>
#include "BlackCrow.h"
#include "SquadUnit.h"

namespace BlackCrow {

	class SquadUnit;
	class Squad;

	typedef std::shared_ptr<SquadUnit> SquadUnitPtr;
	typedef std::shared_ptr<Squad> SquadPtr;

	class Army {
	public:
		Army(BlackCrow &blackrow);

		std::vector<SquadUnitPtr> units;

		void addUnit(BWAPI::Unit unit);
		SquadPtr assignToSquad(SquadUnitPtr);

	private:
		BlackCrow &bc;
	};

}