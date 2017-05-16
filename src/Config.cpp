#include <BWAPI.h>
#include "Config.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Config::Config() {
		localSpeed = 8;
		maxDrones = 200;
		saturationLevel = 0.5;
		mineralSaturationMultiplier = 2;
		barSize = 3;
	}
}