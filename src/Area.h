#pragma once
#include <BWAPI.h>
#include <BWEM/bwem.h>
#include "EnemyUnit.h"
#include "Common.h"

namespace BlackCrow {

	class BlackCrow;
	class BWEM::Area;

	class Area {
	public:
		Area(BlackCrow &parent, const BWEM::Area& bwemArea);
		const BWEM::Area& bwemArea;

		std::vector<BWAPI::TilePosition> associatedTiles;
		std::vector<EnemyUnitPtr> enemies;

	private:
		BlackCrow &bc;
	};

	bool operator==(const Area& left, const Area& right);
		
}