#pragma once
#include <BWAPI.h>
#include <memory>
#include "Common.h"

namespace BlackCrow {

	class BlackCrow;

	class Brick {
	public:
		Brick();
		void run();
		
		std::vector<std::function<bool(void)>> requirements;
		std::vector<std::function<bool(void)>> conditions;
		std::vector<std::function<void(void)>> onces;
		std::vector<std::function<void(void)>> repeats;
		std::vector<std::shared_ptr<Brick>> successors;

		// Requirements
		template <class UnaryPredicate>
		void requirement(UnaryPredicate requirement) {
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

		
	};

	namespace Bricks {
		BrickPtr newBrick();
	}
}