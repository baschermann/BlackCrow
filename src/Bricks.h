#pragma once
#include <BWAPI.h>
#include <memory>

namespace BlackCrow {

	class BlackCrow;

	class Brick {
	public:
		Brick();
		void onStart();
		void onFrame();
		
		//std::vector<std::unique_ptr<?????>> requirementPreds;
		std::vector<std::unique_ptr<Brick>> requirementBricks;

		template <class UnaryPredicate>
		void requirement(UnaryPredicate requirement) {
			// save UnaryPredicate in requirementPreds for later calling
		}

		void requirement(Brick requirement) {

		}
	};
}