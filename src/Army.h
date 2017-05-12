#pragma once
#include <BWAPI.h>
#include "BlackCrow.h"

namespace BlackCrow {

	class Army {
	public:
		Army(BlackCrow &blackrow);

	private:
		BlackCrow &bc;
	};

}