#include <memory>

namespace BlackCrow {
	class Worker;
	class SquadUnit;
	class Squad;
	class UnitMix;
	class PlannedUnit;
	class PlannedBuilding;
	class PlannedExtractor;
	class PlannedUpgrade;
	class PlannedTech;
	class Brick;
	class EnemyUnit;
	class Area;
	struct PathResult;

	typedef std::shared_ptr<Worker> WorkerPtr;
	typedef std::shared_ptr<SquadUnit> SquadUnitPtr;
	typedef std::shared_ptr<Squad> SquadPtr;
	typedef std::shared_ptr<UnitMix> UnitMixPtr;
	typedef std::shared_ptr<PlannedUnit> PlannedUnitPtr;
	typedef std::shared_ptr<PlannedBuilding> PlannedBuildingPtr;
	typedef std::shared_ptr<PlannedExtractor> PlannedExtractorPtr;
	typedef std::shared_ptr<PlannedUpgrade> PlannedUpgradePtr;
	typedef std::shared_ptr<PlannedTech> PlannedTechPtr;
	typedef std::shared_ptr<Brick> BrickPtr;
	typedef std::shared_ptr<EnemyUnit> EnemyUnitPtr;
	typedef std::shared_ptr<Area> AreaPtr;
	typedef std::unique_ptr<PathResult> PathResultUptr;

	typedef std::pair<unsigned int, unsigned int> PairUint;
}