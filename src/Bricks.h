#pragma once
#include <BWAPI.h>
#include <memory>
#include "Common.h"

namespace BlackCrow {

	class BlackCrow;

	class Brick {
	public:
		Brick(BlackCrow& blackcrow, std::string desc);
		void run();
		
		std::vector<std::function<bool(void)>> requirements;
		std::vector<std::function<bool(void)>> conditions;
		std::vector<std::function<void(void)>> onces;
		std::vector<std::function<void(void)>> repeats;
		std::vector<std::shared_ptr<Brick>> successors;
		std::vector<std::shared_ptr<Brick>> disablers;

		// Misc
		void setDescription(std::string description);
		std::string getDescription();
		bool hasRequirementsMet();

		// Requirements
		template <class UnaryPredicate>
		void requiredOnce(UnaryPredicate requirement) {
			requirements.push_back(requirement);
		}
		
		// Conditions
		template <class UnaryPredicate>
		void condition(UnaryPredicate condition) {
			conditions.push_back(requirement);
		}

		// Actions
		template <class UnaryFunction>
		void once(UnaryFunction action) {
			onces.push_back(action);
		}

		template <class UnaryFunction>
		void repeat(UnaryFunction action) {
			repeats.push_back(action);
		}

		// Successor
		void successor(BrickPtr requirement);

		// Disable
		void disableSelfWhenActive(BrickPtr disabler);

	private:
		BlackCrow& bc;
		std::string description;
		bool requirementsMet = false;
		bool oncesHaveRun = false;
		bool disabled = false;

		bool checkConditions();
		bool checkDisablers();
	};

	namespace Bricks {
		BrickPtr makeBlank(BlackCrow& bc, std::string description);
		BrickPtr makePlanUnitOnce(BlackCrow& bc, std::string description, BWAPI::UnitType type, BWAPI::Position nearTo, BrickPtr predecessor);
		BrickPtr makePlanBuildingOnce(BlackCrow& bc, std::string description, BWAPI::UnitType type, BWAPI::TilePosition buildPosition, BrickPtr predecessor);
		BrickPtr makePlanExtractorOnce(BlackCrow& bc, std::string description, const BrickPtr predecessor);
	}
}