#pragma once
#include <BWAPI.h>
#include <iostream>
#include <assert.h>
#include <chrono>
#include "Macro.h"
#include "Strategy.h"
#include "Map.h"
#include "Debug.h"
#include "Enemy.h"
#include "Config.h"
#include "Builder.h"
#include "Util.h"
#include "Tech.h"


// Remember not to use "Broodwar" in any global class constructor!
namespace BlackCrow {

	class BlackCrow : public BWAPI::AIModule {
	public:
		// Virtual functions for callbacks, leave these as they are.
		virtual void onStart();
		virtual void onEnd(bool isWinner);
		virtual void onFrame();
		virtual void onSendText(std::string text);
		virtual void onReceiveText(BWAPI::Player player, std::string text);
		virtual void onPlayerLeft(BWAPI::Player player);
		virtual void onNukeDetect(BWAPI::Position target);
		virtual void onUnitDiscover(BWAPI::Unit unit);
		virtual void onUnitEvade(BWAPI::Unit unit);
		virtual void onUnitShow(BWAPI::Unit unit);
		virtual void onUnitHide(BWAPI::Unit unit);
		virtual void onUnitCreate(BWAPI::Unit unit);
		virtual void onUnitDestroy(BWAPI::Unit unit);
		virtual void onUnitMorph(BWAPI::Unit unit);
		virtual void onUnitRenegade(BWAPI::Unit unit);
		virtual void onSaveGame(std::string gameName);
		virtual void onUnitComplete(BWAPI::Unit unit);
		// Everything below this line is safe to modify.

		BWEM::Map &bwem;
		Builder builder;
		Config config;
		Debug debug;
		Enemy enemy;
		Macro macro;
		Map map;
		Strategy strategy;
		Tech tech;

		BlackCrow();
		void init();
		bool isExecutingCommandFrame();
		void testPath();

	private:
		std::chrono::high_resolution_clock::time_point lastFrame;
	};
}