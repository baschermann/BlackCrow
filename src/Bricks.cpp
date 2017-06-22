#include "Bricks.h"
#include "BlackCrow.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Brick::Brick(BlackCrow& blackcrow, std::string desc) : bc(blackcrow), description(desc) {}

	void Brick::run() {
		
		if (!disabled)
			if (checkDisablers())
				disabled = true; // Use std algorithm any_of etc.

		if (!disabled) {

			// Requirements
			if (!requirementsMet) {
				for (auto requirement : requirements) {
					if (!requirement())
						return;
				}

				requirementsMet = true;
			}

			// Condition and Actions
			if (checkConditions()) {

				if (!oncesHaveRun) {
					for (auto action : onces)
						action();
					oncesHaveRun = true;
				}

				for (auto action : repeats)
					action();
			}

			int minerals = bc.macro.getUnreservedResources().minerals;

			// Run Successors
			for (auto successor : successors)
				successor->run();
		}
	}

	void Brick::successor(BrickPtr successor) {
		successors.push_back(successor);
	}

	bool Brick::checkConditions() {
		for (auto condition : conditions) {
			if (!condition())
				return false;
		}

		return true;
	}

	bool Brick::checkDisablers() {
		for (auto disabler : disablers) {
			if (disabler->hasRequirementsMet())
				return true;
		}

		return false;
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

	void Brick::disableSelfWhenActive(BrickPtr disabler) {
		disablers.push_back(disabler);
	}

	// Brick Namespace
	namespace Bricks {

		BrickPtr makeBlank(BlackCrow& bc, std::string description) {
			return std::make_shared<Brick>(bc, description);
		}

		BrickPtr makePlanUnitOnce(BlackCrow& bc, std::string description, BWAPI::UnitType type, const BWAPI::Position nearTo, const BrickPtr predecessor) {
			BrickPtr brick = makeBlank(bc, description);

			brick->requiredOnce([&bc, type]() {
				auto resources = bc.macro.getUnreservedResources();
				return resources.minerals >= type.mineralPrice() && resources.gas >= type.gasPrice() && bc.macro.getUnreservedLarvaeAmount() >= 1;
			});

			brick->once([&bc, type, nearTo, description]() { 
				bc.macro.planUnit(type, nearTo); 
			});

			if (predecessor)
				predecessor->successor(brick);

			return brick;
		}

		BrickPtr makePlanBuildingOnce(BlackCrow& bc, std::string description, BWAPI::UnitType type, BWAPI::TilePosition buildSearchStart, bool inMineralLine, BrickPtr predecessor) {
			BrickPtr brick = makeBlank(bc, description);

			brick->requiredOnce([&bc, type]() {
				auto resources = bc.macro.getUnreservedResources();
				return resources.minerals >= type.mineralPrice() && resources.gas >= type.gasPrice();
			});

			brick->once([&bc, type, buildSearchStart]() { bc.macro.planBuilding(type, bc.builder.getBuildingSpot(type, buildSearchStart, false)); });

			if (predecessor)
				predecessor->successor(brick);

			return brick;
		}

		BrickPtr makePlanExtractorOnce(BlackCrow& bc, std::string description, const BrickPtr predecessor) {
			BrickPtr brick = makeBlank(bc, description);

			brick->requiredOnce([&bc]() {
				auto resources = bc.macro.getUnreservedResources();
				return resources.minerals >= UnitTypes::Zerg_Extractor.mineralPrice();
			});

			brick->once([&bc]() { bc.macro.buildExtractor(); });

			if (predecessor)
				predecessor->successor(brick);

			return brick;
		}
	}
}