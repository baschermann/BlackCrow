#include "Strategy.h"
#include "BlackCrow.h"
#include <BWEM/bwem.h>
#include "Area.h"
#include "Planned.h"
#include "Macro.h"
#include "Bricks.h"
#include "Common.h"


namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Strategy::Strategy(BlackCrow &parent) : bc(parent), start("START") {
		unitMix = std::make_unique<UnitMix>(bc);
	}

	void Strategy::onStart() {
		unitMix->set(BWAPI::UnitTypes::Zerg_Drone, 1, true);

		// Initial Scout
		BrickPtr initialScout = Bricks::makeBlank("Initial Scout");
		initialScout->requiredOnce([&bc = bc]() { return bc.macro.getUsedSupply() >= 18; });
		initialScout->onceAfterRequirements([&bc = bc]() { bc.army.startInitialScout(); });
		start.runAfterRequirements(initialScout);
		

		// ##### Rush Defense #####
		BrickPtr rushDefense = Bricks::makeBlank("Rush Defense");
		rushDefense->requiredOnce([&bc = bc]() { return bc.map.getNearestArea(TilePosition(bc.macro.startPosition))->enemies.size() >= 2; });
		//rushDefense->once([]() { Broodwar->sendText("Rush defense activated!"); });
		start.runAfterRequirements(rushDefense);

		// Build a spawning pool immedietly
		BrickPtr rushDefenseSpawningPool = Bricks::makeBlank("Rush Defense: Build Spawning Pool");
		rushDefenseSpawningPool->condition([&bc = bc]() { return bc.macro.hasAmountOf(UnitTypes::Zerg_Spawning_Pool) <= 0 && bc.macro.getCurrentlyPlannedAmount(UnitTypes::Zerg_Spawning_Pool) <= 0; });
		rushDefenseSpawningPool->repeatWhenTrue([&bc = bc]() { bc.macro.planBuilding(UnitTypes::Zerg_Spawning_Pool, bc.builder.getBuildingSpot(UnitTypes::Zerg_Spawning_Pool)); });
		rushDefense->runAfterRequirements(rushDefenseSpawningPool);

		// Build Overlords
		BrickPtr rushDefenseBuildOverlords = Bricks::makeBlank("Rush Defense: Build Overlords");
		rushDefenseBuildOverlords->condition([&bc = bc]() { return bc.macro.getFreeSupply() <= 0; });
		rushDefenseBuildOverlords->repeatWhenTrue([&bc = bc]() { bc.macro.planUnit(UnitTypes::Zerg_Overlord, bc.macro.startPosition); });
		rushDefense->runAfterRequirements(rushDefenseBuildOverlords);

		// Stop a planned overlord if we have free supply
		BrickPtr rushDefenseCancelOverlords = Bricks::makeBlank("Rush Defense: Cancel non started planned Overlords");
		rushDefenseCancelOverlords->condition([&bc = bc]() { return bc.macro.getFreeSupply() >= 1 && bc.macro.getCurrentlyPlannedAmount(UnitTypes::Zerg_Overlord) >= 1; });
		rushDefenseCancelOverlords->repeatWhenTrue([&bc = bc]() {
			for (auto pu : bc.macro.getPlannedUnits()) {
				if (pu->type == UnitTypes::Zerg_Overlord && !pu->unit)
					bc.macro.removePlanned(pu);
			}
		});
		rushDefense->runAfterRequirements(rushDefenseCancelOverlords);

		// Build Zerglings
		BrickPtr rushDefenseBuildZerglings = Bricks::makeBlank("Rush Defense: Build Zerglings");
		rushDefenseBuildZerglings->condition([&bc = bc]() {
			return Broodwar->canMake(UnitTypes::Zerg_Zergling)
				&& bc.macro.getUnreservedResources().minerals >= UnitTypes::Zerg_Zergling.mineralPrice()
				&& bc.macro.getUnreservedLarvaeAmount() >= 1;
		});
		rushDefenseBuildZerglings->repeatWhenTrue([&bc = bc]() { bc.macro.planUnit(UnitTypes::Zerg_Zergling, bc.macro.startPosition); });
		rushDefense->runAfterRequirements(rushDefenseBuildZerglings);

		// # Rush defense end #
		BrickPtr rushDefenseEnd = Bricks::makeBlank("Rush Defense End");
		rushDefenseEnd->requiredOnce([rushDefense]() { return rushDefense->isActive(); });
		rushDefenseEnd->requiredOnce([&bc = bc]() { return bc.map.getNearestArea(TilePosition(bc.macro.startPosition))->enemies.size() == 0; });
		//rushDefenseEnd->once([]() { Broodwar->sendText("Rush Defense ended"); });
		rushDefense->disableSelfWhenActive(rushDefenseEnd);
		start.runAfterRequirements(rushDefenseEnd);

		// ##### Buildorders against enemy Race #####
		// Protoss
		BrickPtr protossBo = Bricks::makeBlank("Enemy Protoss BO");
		protossBo->requiredOnce([]() { return Broodwar->enemy()->getRace().getName() == "Protoss"; });
		start.runAfterRequirements(protossBo);
		BrickPtr protossBoLast = buildorderOverpool(protossBo);

		// Terran
		BrickPtr terranBo = Bricks::makeBlank("Enemy Terran BO");
		terranBo->requiredOnce([]() { return Broodwar->enemy()->getRace().getName() == "Terran"; });
		start.runAfterRequirements(terranBo);
		BrickPtr terranBoLast = buildorderTwelveHatch(terranBo);

		// Zerg
		BrickPtr zergBo = Bricks::makeBlank("Enemy Zerg BO");
		zergBo->requiredOnce([]() { return Broodwar->enemy()->getRace().getName() == "Zerg"; });
		start.runAfterRequirements(zergBo);
		BrickPtr zergBoLast = buildorderNinePool(zergBo);

		// Random
		BrickPtr randomBo = Bricks::makeBlank("Random BO");
		randomBo->requiredOnce([]() { return Broodwar->enemy()->getRace().getName() == "Unknown"; });
		start.runAfterRequirements(randomBo);
		BrickPtr randomBoLast = buildorderOverpool(randomBo);

		// Disable race specific openings when enemy plays random
		protossBo->disableSelfWhenActive(randomBo);
		terranBo->disableSelfWhenActive(randomBo);
		zergBo->disableSelfWhenActive(randomBo);

		// Disable all openings when rush defence is active
		protossBo->disableSelfWhenActive(rushDefense);
		terranBo->disableSelfWhenActive(rushDefense);
		zergBo->disableSelfWhenActive(rushDefense);
		randomBo->disableSelfWhenActive(rushDefense);

		// ##### Go dynamic #####
		BrickPtr dynamicStart = Bricks::makeBlank("Dynamic decisions");
		//dynamicStart->onceAfterRequirements([]() { Broodwar->sendText("Dynamic stuff starts here!"); });

		protossBoLast->runAfterRequirements(dynamicStart);
		terranBoLast->runAfterRequirements(dynamicStart);
		zergBoLast->runAfterRequirements(dynamicStart);
		randomBoLast->runAfterRequirements(dynamicStart);
		rushDefenseEnd->runAfterRequirements(dynamicStart);

		// Give up
		BrickPtr giveUp = Bricks::makeBlank("Give up");
		giveUp->requiredOnce([]() { return Broodwar->self()->minerals() < 50 && Broodwar->self()->supplyUsed() <= 0; });
		giveUp->onceAfterRequirements([]() { 
			Broodwar->sendText("gaw gaw!");	
			Broodwar->leaveGame(); 
		});

		// Use Standard Strategy for now
		standardStrategy(dynamicStart);
	}

	BrickPtr Strategy::buildorderOverpool(BrickPtr& predecessor) {
		Bricks::SucessorInPredecessorChain chain(predecessor);
		chain.set(Bricks::makePlanUnitOnce		(bc, "Drone @4", UnitTypes::Zerg_Drone, bc.macro.startPosition));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Drone @5", UnitTypes::Zerg_Drone, bc.macro.startPosition));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Drone @6", UnitTypes::Zerg_Drone, bc.macro.startPosition));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Drone @7", UnitTypes::Zerg_Drone, bc.macro.startPosition));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Drone @8", UnitTypes::Zerg_Drone, bc.macro.startPosition));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Overlord @9", UnitTypes::Zerg_Overlord, bc.macro.startPosition));
		chain.set(Bricks::makePlanBuildingOnce	(bc, "Pool @9", UnitTypes::Zerg_Spawning_Pool, TilePosition(bc.macro.startPosition), false));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Drone @8", UnitTypes::Zerg_Drone, bc.macro.startPosition));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Drone @9", UnitTypes::Zerg_Drone, bc.macro.startPosition));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Drone @10", UnitTypes::Zerg_Drone, bc.macro.startPosition));
		chain.set(Bricks::makePlanExtractorOnce	(bc, "Extractor @11")); // Go parallel for case extractor blocked?
		chain.set(Bricks::makePlanUnitOnce		(bc, "Zergling @10", UnitTypes::Zerg_Zergling, bc.macro.startPosition));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Zergling @10", UnitTypes::Zerg_Zergling, bc.macro.startPosition));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Zergling @10", UnitTypes::Zerg_Zergling, bc.macro.startPosition));
		return chain.getLastSuccessor();
	}

	BrickPtr Strategy::buildorderTwelveHatch(BrickPtr& predecessor) {
		Bricks::SucessorInPredecessorChain chain(predecessor);
		chain.set(Bricks::makePlanUnitOnce		(bc, "Drone @4", UnitTypes::Zerg_Drone, bc.macro.startPosition));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Drone @5", UnitTypes::Zerg_Drone, bc.macro.startPosition));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Drone @6", UnitTypes::Zerg_Drone, bc.macro.startPosition));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Drone @7", UnitTypes::Zerg_Drone, bc.macro.startPosition));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Drone @8", UnitTypes::Zerg_Drone, bc.macro.startPosition));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Overlord @9", UnitTypes::Zerg_Overlord, bc.macro.startPosition));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Drone @9", UnitTypes::Zerg_Drone, bc.macro.startPosition));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Drone @10", UnitTypes::Zerg_Drone, bc.macro.startPosition));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Drone @11", UnitTypes::Zerg_Drone, bc.macro.startPosition));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Drone @12", UnitTypes::Zerg_Drone, bc.macro.startPosition));
		chain.set(Bricks::makePlanBuildingOnce	(bc, "Hatchery @12", UnitTypes::Zerg_Hatchery, TilePosition(bc.macro.startPosition), false));
		chain.set(Bricks::makePlanBuildingOnce	(bc, "Pool @11", UnitTypes::Zerg_Spawning_Pool, TilePosition(bc.macro.startPosition), false));
		chain.set(Bricks::makePlanExtractorOnce	(bc, "Extractor @10")); // Go parallel for case extractor blocked?
		chain.set(Bricks::makePlanUnitOnce		(bc, "Zergling @10", UnitTypes::Zerg_Zergling, bc.macro.startPosition));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Zergling @10", UnitTypes::Zerg_Zergling, bc.macro.startPosition));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Zergling @10", UnitTypes::Zerg_Zergling, bc.macro.startPosition));
		return chain.getLastSuccessor();
	}

	BrickPtr Strategy::buildorderNinePool(BrickPtr& predecessor) {
		Bricks::SucessorInPredecessorChain chain(predecessor);
		chain.set(Bricks::makePlanUnitOnce		(bc, "Drone @4", UnitTypes::Zerg_Drone, bc.macro.startPosition));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Drone @5", UnitTypes::Zerg_Drone, bc.macro.startPosition));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Drone @6", UnitTypes::Zerg_Drone, bc.macro.startPosition));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Drone @7", UnitTypes::Zerg_Drone, bc.macro.startPosition));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Drone @8", UnitTypes::Zerg_Drone, bc.macro.startPosition));
		chain.set(Bricks::makePlanBuildingOnce	(bc, "Pool @9", UnitTypes::Zerg_Spawning_Pool, TilePosition(bc.macro.startPosition), false));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Drone @8", UnitTypes::Zerg_Drone, bc.macro.startPosition));
		chain.set(Bricks::makePlanExtractorOnce	(bc, "Extractor @9")); // Go parallel for case extractor blocked?
		chain.set(Bricks::makePlanUnitOnce		(bc, "Overlord @8", UnitTypes::Zerg_Overlord, bc.macro.startPosition));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Drone @8", UnitTypes::Zerg_Drone, bc.macro.startPosition));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Zergling @10", UnitTypes::Zerg_Zergling, bc.macro.startPosition));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Zergling @10", UnitTypes::Zerg_Zergling, bc.macro.startPosition));
		chain.set(Bricks::makePlanUnitOnce		(bc, "Zergling @10", UnitTypes::Zerg_Zergling, bc.macro.startPosition));
		return chain.getLastSuccessor();
	}

	void Strategy::onFrame() {
		start.run();
	}

	void Strategy::standardStrategy(BrickPtr& predecessor) {
		
		// Build a spawning pool if none exists
		BrickPtr buildSpawningPool = Bricks::makeBlank("Build spawning pool");
		buildSpawningPool->condition([&bc = bc]() {
			return bc.macro.hasAmountOf(UnitTypes::Zerg_Spawning_Pool) <= 0
				&& bc.macro.getCurrentlyPlannedAmount(UnitTypes::Zerg_Spawning_Pool) <= 0;
		});
		buildSpawningPool->repeatWhenTrue([&bc = bc]() { bc.macro.planBuilding(UnitTypes::Zerg_Spawning_Pool, TilePosition(bc.macro.startPosition)); });

		// Drones in the unitMix
		BrickPtr dronesInUnitMix = Bricks::makeBlank("Add/Remove drones in UnitMix");
		dronesInUnitMix->condition([&bc = bc]() { 
			return bc.macro.getWorkersNeededForSaturation() - bc.macro.getCurrentlyPlannedAmount(UnitTypes::Zerg_Drone) > 0 
				|| bc.macro.getTotalWorkerAmount() >= bc.config.maxDrones; 
		});
		dronesInUnitMix->repeatWhenTrue([&unitMix = unitMix]() {
			if(!unitMix->exists(UnitTypes::Zerg_Drone))
				unitMix->set(UnitTypes::Zerg_Drone, 1, true);
		});
		dronesInUnitMix->repeatWhenFalse([&unitMix = unitMix]() {
			if (unitMix->exists(UnitTypes::Zerg_Drone))
				unitMix->remove(UnitTypes::Zerg_Drone);
		});
		predecessor->runAfterRequirements(dronesInUnitMix);

		// Collect 100 Gas
		BrickPtr collectGas = Bricks::makeBlank("Collect 100 Gas");
		collectGas->condition([]() { return Broodwar->self()->gas() + Broodwar->self()->spentGas() < 100; });
		collectGas->repeatWhenTrue([&bc = bc]() { 
			if (bc.macro.getGasWorkerSlotsAvailable() > 0)
				if (bc.macro.getTotalGasWorkerAmount() < 3)
					bc.macro.addGasWorker();
		});
		collectGas->repeatWhenFalse([&bc = bc]() {
			if (bc.macro.getTotalGasWorkerAmount() > 0)
				bc.macro.removeGasWorker();
		});
		predecessor->runAfterRequirements(collectGas);

		// Check if we have a finished spawning pool
		BrickPtr spawningPoolStuff = Bricks::makeBlank("Own a completed Spawning Pool");
		spawningPoolStuff->requiredOnce([]() {
			auto ownUnits = Broodwar->self()->getUnits();
			auto spawnPoolIt = std::find_if(ownUnits.begin(), ownUnits.end(), [](Unit unit) { return unit->getType() == UnitTypes::Zerg_Spawning_Pool && unit->isCompleted(); });
			return spawnPoolIt != ownUnits.end();
		});
		predecessor->runAfterRequirements(spawningPoolStuff);
		// Zerglings Speed
		BrickPtr zerglingSpeed = Bricks::makeBlank("Zergling Speed");
		zerglingSpeed->condition([&bc = bc]() {
			return bc.macro.getUnreservedResources().gas >= 100 
				&& !bc.macro.isCurrentlyPlanned(UpgradeTypes::Metabolic_Boost) 
				&& Broodwar->self()->getUpgradeLevel(UpgradeTypes::Metabolic_Boost) <= 0;
		});
		zerglingSpeed->repeatWhenTrue([&bc = bc]() { bc.macro.planUpgrade(UpgradeTypes::Metabolic_Boost, 1); });
		spawningPoolStuff->runAfterRequirements(zerglingSpeed);
		// Add Zerglings to UnitMix
		BrickPtr addZerglings = Bricks::makeBlank("Add Zerglings into unitMix");
		addZerglings->onceAfterRequirements([&unitMix = unitMix]() {
			unitMix->set(UnitTypes::Zerg_Zergling, 2, true);
		});
		spawningPoolStuff->runAfterRequirements(addZerglings);


		// ### Build units from unitMix ###
		BrickPtr checkUnitMix = Bricks::makeBlank("Unit Mix");
		checkUnitMix->condition([&unitMix = unitMix]() { return unitMix->size() > 0; });
		predecessor->runAfterRequirements(checkUnitMix);

		// Calculate some data from the unit mix based on current economy
		struct UnitMixData {
			double productionMultiplierMinerals = 0;
			double productionMultiplierGas = 0;
			double productionMultiplierLarvae = 0;
			double productionMultiplier = 0;

			void update(BlackCrow& bc, UnitMix& unitMix) {
				productionMultiplierMinerals = bc.macro.getAverageMineralsPerFrame() / unitMix.mineralPerFrame();
				//productionMultiplierGas = bc.macro.getAverageGasPerFrame() / unitMix->gasPerFrame(); // TODO Gas units not included
				productionMultiplierLarvae = bc.macro.getAverageLarvaePerFrame() / unitMix.larvaPerFrame();
				productionMultiplier = std::min(productionMultiplierMinerals, productionMultiplierLarvae);
			}
		};

		// Make a new unitMixData and update it every frame
		auto unitMixData = std::make_shared<UnitMixData>();
		checkUnitMix->repeatAfterRequirements([&bc = bc, &unitMix = unitMix, unitMixData]() { unitMixData->update(bc, *unitMix); });

		// Build Supply
		BrickPtr buildSupply = Bricks::makeBlank("Build supply");
		buildSupply->condition([&bc = bc, &unitMix = unitMix, unitMixData]() {
			return bc.macro.getFreeSupply() / (unitMix->supplyPerFrame() * unitMixData->productionMultiplier) < UnitTypes::Zerg_Overlord.buildTime()
				&& bc.macro.getFreeSupply() <= 20;
		});
		buildSupply->repeatWhenTrue([&bc = bc]() { bc.macro.planUnit(UnitTypes::Zerg_Overlord, bc.macro.startPosition); });
		checkUnitMix->runAfterRequirements(buildSupply);

		// Build Units from Unit Mix
		BrickPtr buildUnits = Bricks::makeBlank("Build units");
		buildUnits->condition([&bc = bc, &unitMix = unitMix]() {
			return bc.macro.getUnreservedResources().minerals >= unitMix->peek().mineralPrice()
				&& bc.macro.getUnreservedLarvaeAmount() > 0;
		});
		buildUnits->repeatWhenTrue([&bc = bc, &unitMix = unitMix]() {
			if (unitMix->peek() == UnitTypes::Zerg_Drone) {
				bc.macro.buildWorkerDrone();
				unitMix->set(UnitTypes::Zerg_Drone, std::max(unitMix->get(UnitTypes::Zerg_Drone) - 0.1, 0.15), false);
				unitMix->pop();
			} else {
				bc.macro.planUnit(unitMix->pop(), bc.macro.startPosition);
			}
		});
		checkUnitMix->runAfterRequirements(buildUnits);

		// Additional Hatcheries
		BrickPtr buildhatcheries = Bricks::makeBlank("Build hatcheries");
		buildhatcheries->condition([&bc = bc, unitMixData]() {
			return unitMixData->productionMultiplierLarvae < unitMixData->productionMultiplierMinerals
				&& bc.macro.getTotalLarvaeAmount() <= 0;
		});
		buildhatcheries->repeatWhenTrue([&bc = bc]() {
			int amountPlannedHatcheries = bc.macro.getCurrentlyPlannedAmount(UnitTypes::Zerg_Hatchery);

			if (amountPlannedHatcheries <= 0) {
				if (bc.macro.getUnreservedResources().minerals >= 250)
					bc.macro.planBuilding(UnitTypes::Zerg_Hatchery, bc.builder.getBuildingSpot(UnitTypes::Zerg_Hatchery));
			}
			else {
				if ((double)bc.macro.getUnreservedResources().minerals / ((double)amountPlannedHatcheries * (double)300) >= 0.75)
					bc.macro.planBuilding(UnitTypes::Zerg_Hatchery, bc.builder.getBuildingSpot(UnitTypes::Zerg_Hatchery));
			}
		});
		checkUnitMix->runAfterRequirements(buildhatcheries);
	}
}