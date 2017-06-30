#pragma once
#include <BWAPI.h>
#include <list>
#include <memory>
#include "Squad.h" // Can this be moved to .cpp?
#include "UnitMix.h" // Needed for UnitMixPtr which is a unique_ptr, can this be moved to .cpp?
#include "Bricks.h"



/////////////////////////////////////////////////////
// General Questions, summarized by IronfistJaiden //
/////////////////////////////////////////////////////

//// questions i think every bot wants to be able to answer(and few / none do)
// What does my opponent have ?
// Where are my opponent's units?
// What would happen if these units fought ?
// What can I and my opponent do X minutes from now ?
//
//// Out of the answers to those questions flow the answers to everything else :
// Can I kill my opponent right now ?
// Do I need to build additional defense to survive ?
// If I build all units, will I be able to kill my opponent ?
// Do I need to take risks to get ahead ?

/////////////////////////////////////////////////////

namespace BlackCrow {

	class BlackCrow;
	class PlannedUnit;
	class BaseInformation;
	class ScoutSquad;
	class Area;
	class Planned;
	class UnitMix;

	class Strategy {
	public:
		// General
		Strategy(BlackCrow &parent);
		void onStart();
		void onFrame();

		// Unit Mix
		UnitMixPtr unitMix = nullptr;

		// Decision decisions
		double productionMultiplierMinerals = 0;
		double productionMultiplierGas = 0;
		double productionMultiplierLarvae = 0;
		double productionMultiplier = 0;
		
	private:
		BlackCrow &bc;
		bool initialScoutStarted = false;
		Brick start;

		BrickPtr buildorderOverpool(BrickPtr& predecessor);
		BrickPtr buildorderTwelveHatch(BrickPtr& predecessor);
		BrickPtr buildorderNinePool(BrickPtr& predecessor);
		void standardStrategy(BrickPtr& predecessor);
	};
}