#include "Bricks.h"
#include "BlackCrow.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Brick::Brick(std::string desc) : description(desc) {}

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

		BrickPtr makeBlank(std::string description) {
			return std::make_shared<Brick>(description);
		}

		BrickPtr makePlanUnitOnce(BlackCrow& bc, std::string description, BWAPI::UnitType type, const BWAPI::Position nearTo) {
			BrickPtr brick = makeBlank(description);

			brick->requiredOnce([&bc, type]() {
				auto resources = bc.macro.getUnreservedResources();
				return resources.minerals >= type.mineralPrice() && resources.gas >= type.gasPrice() && bc.macro.getUnreservedLarvaeAmount() >= 1;
			});

			brick->once([&bc, type, nearTo, description]() { 
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

			brick->once([&bc, type, buildSearchStart]() { bc.macro.planBuilding(type, bc.builder.getBuildingSpot(type, buildSearchStart, false)); });

			return brick;
		}

		BrickPtr makePlanExtractorOnce(BlackCrow& bc, std::string description) {
			BrickPtr brick = makeBlank(description);

			brick->requiredOnce([&bc]() {
				auto resources = bc.macro.getUnreservedResources();
				return resources.minerals >= UnitTypes::Zerg_Extractor.mineralPrice();
			});

			brick->once([&bc]() { bc.macro.buildExtractor(); });

			return brick;
		}
	}
}