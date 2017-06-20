#include "Bricks.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Brick::Brick(std::string desc) : description(desc) {}

	void Brick::run() {
		
		if (!disabled)
			if (checkDisablers())
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
		BrickPtr newBrick(std::string description) {
			return std::make_shared<Brick>(description);
		}
	}
}