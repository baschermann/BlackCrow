#include "Bricks.h"
#include "BlackCrow.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Brick::Brick(std::string desc) : description(desc) {}

	void Brick::run() {
		
		if (!disabled)
			if (std::any_of(disablers.begin(), disablers.end(), [](BrickPtr& brick) { return brick->hasRequirementsMet(); }))
				disabled = true;

		if (!disabled) {

			// Requirements
			if (!requirementsMet) {
				for (auto requirement : requirements) {
					if (!requirement())
						return;
				}

				requirementsMet = true;
			}

			// Run onces after requirements
			if (!oncesRequirementHaveRun) {
				for (auto& action : oncesRequirement)
					action();
				oncesRequirementHaveRun = true;
			}

			// run repeats after requirements
			for (auto& action : repeatsRequirement)
				action();


			// Condition
			bool conditions = checkConditions();
			//bool conditions = std::all_of(conditions.begin(), conditions.end(), []() {}); // TODO how does this work?
			if (conditions) {

				// Onces when true
				if (!oncesTrueHaveRun) {
					for (auto& action : oncesTrue)
						action();
					oncesTrueHaveRun = true;
				}

				// Repeats when true
				for (auto& action : repeatsTrue)
					action();

			} else {
				// Onces when false
				if (!oncesFalseHaveRun) {
					for (auto& action : oncesFalse)
						action();
					oncesFalseHaveRun = true;
				}

				// Repeat when false
				for (auto& action : repeatsFalse)
					action();
			}

			// Run Successors after Requirements
			for (auto& successor : successorsRequirement)
				successor->run();

			if(conditions)
				// Run Successors when conditions are true
				for (auto& successor : successorsTrue)
					successor->run();
			else
				// Run Successors when conditions are false
				for (auto& successor : successorsFalse)
					successor->run();
		}
	}

	void Brick::runAfterRequirements(BrickPtr& successor) {
		successorsRequirement.push_back(successor);
	}

	void Brick::runWhenTrue(BrickPtr& successor) {
		successorsTrue.push_back(successor);
	}

	void Brick::runWhenFalse(BrickPtr& successor) {
		successorsFalse.push_back(successor);
	}

	bool Brick::checkConditions() {
		for (auto& condition : conditions)
			if (!condition())
				return false;
		return true;
	}

	void Brick::setDescription(std::string des) {
		description = des;
	}

	std::string Brick::getDescription() {
		return description;
	}

	bool Brick::hasRequirementsMet() {
		return requirementsMet;
	}

	void Brick::disableSelfWhenActive(BrickPtr& disabler) {
		disablers.push_back(disabler);
	}

	// Brick Namespace
	namespace Bricks {

		BrickPtr makeBlank(std::string description) {
			return std::make_shared<Brick>(description);
		}

		BrickPtr makePlanUnitOnce(BlackCrow& bc, std::string description, BWAPI::UnitType type, const BWAPI::Position nearTo) {
			BrickPtr brick = makeBlank(description);

			brick->requiredOnce([&bc, type]() {
				auto resources = bc.macro.getUnreservedResources();
				return resources.minerals >= type.mineralPrice() && resources.gas >= type.gasPrice() && bc.macro.getUnreservedLarvaeAmount() >= 1;
			});

			brick->onceAfterRequirements([&bc, type, nearTo, description]() { 
				bc.macro.planUnit(type, nearTo); 
			});

			return brick;
		}

		BrickPtr makePlanBuildingOnce(BlackCrow& bc, std::string description, BWAPI::UnitType type, BWAPI::TilePosition buildSearchStart, bool inMineralLine) {
			BrickPtr brick = makeBlank(description);

			brick->requiredOnce([&bc, type]() {
				auto resources = bc.macro.getUnreservedResources();
				return resources.minerals >= type.mineralPrice() && resources.gas >= type.gasPrice();
			});

			brick->onceAfterRequirements([&bc, type, buildSearchStart]() { bc.macro.planBuilding(type, bc.builder.getBuildingSpot(type, buildSearchStart, false)); });

			return brick;
		}

		BrickPtr makePlanExtractorOnce(BlackCrow& bc, std::string description) {
			BrickPtr brick = makeBlank(description);

			brick->requiredOnce([&bc]() {
				auto resources = bc.macro.getUnreservedResources();
				return resources.minerals >= UnitTypes::Zerg_Extractor.mineralPrice();
			});

			brick->onceAfterRequirements([&bc]() { bc.macro.buildExtractor(); });

			return brick;
		}
	}
}