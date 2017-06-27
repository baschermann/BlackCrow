#pragma once
#include <BWAPI.h>
#include <memory>
#include "Common.h"

namespace BlackCrow {

	class BlackCrow;

	class Brick {
	public:
		Brick(std::string desc);
		void run();

		// Misc
		void setDescription(std::string description);
		std::string getDescription();
		bool hasRequirementsMet();

		// Requirements
		template <class UnaryPredicate>
		void requiredOnce(UnaryPredicate requirement) {
			requirements.push_back(requirement);
		}

		template <class UnaryFunction>
		void onceAfterRequirements(UnaryFunction action) {
			oncesRequirement.push_back(action);
		}

		template <class UnaryFunction>
		void repeatAfterRequirements(UnaryFunction action) {
			repeatsRequirement.push_back(action);
		}
		
		// Conditions
		template <class UnaryPredicate>
		void condition(UnaryPredicate condition) {
			conditions.push_back(condition);
		}

		template <class UnaryFunction>
		void onceWhenTrue(UnaryFunction action) {
			oncesTrue.push_back(action);
		}

		template <class UnaryFunction>
		void onceWhenFalse(UnaryFunction action) {
			oncesFalse.push_back(action);
		}

		template <class UnaryFunction>
		void repeatWhenTrue(UnaryFunction action) {
			repeatsTrue.push_back(action);
		}

		template <class UnaryFunction>
		void repeatWhenFalse(UnaryFunction action) {
			repeatsFalse.push_back(action);
		}

		// Successor
		void runAfterRequirements(BrickPtr& requirement);
		void runWhenTrue(BrickPtr& requirement);
		void runWhenFalse(BrickPtr& requirement);

		// Disable
		void disableSelfWhenActive(BrickPtr& disabler);

		// Misc
		const bool isActive() {
			return requirementsMet && !disabled;
		}

	private:
		std::vector<std::function<bool(void)>> requirements;
		std::vector<std::function<void(void)>> oncesRequirement;
		std::vector<std::function<void(void)>> repeatsRequirement;
		std::vector<std::function<bool(void)>> conditions;
		std::vector<std::function<void(void)>> oncesTrue;
		std::vector<std::function<void(void)>> oncesFalse;
		std::vector<std::function<void(void)>> repeatsTrue;
		std::vector<std::function<void(void)>> repeatsFalse;
		std::vector<std::shared_ptr<Brick>> successorsRequirement;
		std::vector<std::shared_ptr<Brick>> successorsTrue;
		std::vector<std::shared_ptr<Brick>> successorsFalse;
		std::vector<std::shared_ptr<Brick>> disablers;

		std::string description;
		bool requirementsMet = false;
		bool oncesRequirementHaveRun = false;
		bool oncesTrueHaveRun = false;
		bool oncesFalseHaveRun = false;
		bool disabled = false;
		int lastFrameRun = -1;

		bool checkConditions();
	};

	namespace Bricks {
		struct SucessorInPredecessorChain {
		private:
			BrickPtr pr = nullptr;

		public:
			SucessorInPredecessorChain(BrickPtr& predecessor) {
				pr = predecessor;
			}

			void set(BrickPtr successor) {
				if (pr)
					pr->runAfterRequirements(successor);

				pr = successor;
			};

			BrickPtr getLastSuccessor() {
				return pr;
			}
		};

		BrickPtr makeBlank(std::string description);
		BrickPtr makePlanUnitOnce(BlackCrow& bc, std::string description, BWAPI::UnitType type, BWAPI::Position nearTo);
		BrickPtr makePlanBuildingOnce(BlackCrow& bc, std::string description, BWAPI::UnitType type, BWAPI::TilePosition buildSearchStart, bool inMineralLine);
		BrickPtr makePlanExtractorOnce(BlackCrow& bc, std::string description);
	}
}