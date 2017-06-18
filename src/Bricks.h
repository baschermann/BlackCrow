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
			auto bla = std::make_unique<UnaryPredicate>(requirement);
			(*bla)();
		}

		// If compile error: http://de.cppreference.com/w/cpp/language/template_specialization
		void requirement(Brick requirement) {

		}
	};
}