#pragma once
#include <BWAPI.h>
#include <BWEM/bwem.h>

namespace BlackCrow {
	class Base {
	public:
		Base(BWEM::Base& bwemBase);

		BWEM::Base& bwemBase;
	};

}