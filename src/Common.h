#include <memory>

namespace BlackCrow {
	class Worker;
	class SquadUnit;
	class Squad;
	class ScoutSquad;
	class AttackSquad;
	class UnitMix;

	typedef std::shared_ptr<Worker> WorkerPtr;
	typedef std::shared_ptr<SquadUnit> SquadUnitPtr;
	typedef std::shared_ptr<Squad> SquadPtr;
	typedef std::shared_ptr<ScoutSquad> ScoutSquadPtr;
	typedef std::shared_ptr<AttackSquad> AttackSquadPtr;
	typedef std::unique_ptr<UnitMix> UnitMixPtr;
}