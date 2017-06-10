#include <memory>

namespace BlackCrow {
	class Worker;
	class SquadUnit;
	class Squad;
	class ScoutSquad;
	class AttackSquad;
	class UnitMix;
	class PlannedUnit;
	class PlannedBuilding;
	class PlannedExtractor;
	class PlannedUpgrade;
	class PlannedTech;

	typedef std::shared_ptr<Worker> WorkerPtr;
	typedef std::shared_ptr<SquadUnit> SquadUnitPtr;
	typedef std::shared_ptr<Squad> SquadPtr;
	typedef std::shared_ptr<ScoutSquad> ScoutSquadPtr;
	typedef std::shared_ptr<AttackSquad> AttackSquadPtr;
	typedef std::unique_ptr<UnitMix> UnitMixPtr;
	typedef std::shared_ptr<PlannedUnit> PlannedUnitPtr;
	typedef std::shared_ptr<PlannedBuilding> PlannedBuildingPtr;
	typedef std::shared_ptr<PlannedExtractor> PlannedExtractorPtr;
	typedef std::shared_ptr<PlannedUpgrade> PlannedUpgradePtr;
	typedef std::shared_ptr<PlannedTech> PlannedTechPtr;
}