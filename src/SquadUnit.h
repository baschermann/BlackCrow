#pragma once
#include <BWAPI.h>


class SquadUnit {
public:
	SquadUnit(BWAPI::Unit unit);
	~SquadUnit();

	BWAPI::Unit unit;

	void move(BWAPI::Position position, bool queue);
};