#include "BlackCrow.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	BlackCrow::BlackCrow() : builder(*this), debug(*this), enemy(*this), macro(*this), map(*this), strategy(*this), tech(*this), army(*this), bwem(BWEM::Map::Instance()) {}

	void BlackCrow::onStart() {

		try {

			auto start = std::chrono::high_resolution_clock::now();

			init();

			// BWEM
			bwem.Initialize();
			bwem.EnableAutomaticPathAnalysis();
			bool startingLocationsOK = bwem.FindBasesForStartingLocations();
			assert(startingLocationsOK);

			//BWEM::utils::MapPrinter::Initialize(&bwem);
			//BWEM::utils::printMap(*map);      // will print the map into the file <StarCraftFolder>bwapi-data/map.bmp
			//BWEM::utils::pathExample(*map);   // add to the printed map a path between two starting locations
			// END of BWEM

			map.onStart();
			macro.onStart();
			strategy.onStart();
			enemy.onStart();
			builder.onStart();
			tech.onStart();
			debug.onStart();
			army.onStart();

			auto end = std::chrono::high_resolution_clock::now();

			std::chrono::duration<double, std::milli> diff = end - start;
			//Broodwar->sendText("Init has taken %fms", diff.count());
			//Broodwar->sendText("black sheep wall");



		} catch (const std::exception &e) {
			Broodwar << "EXCEPTION: " << e.what() << std::endl;
		}
	}

	void BlackCrow::onFrame() {
		try {
			auto start = std::chrono::high_resolution_clock::now();

			// Performance Qeuery Timer
			// http://www.decompile.com/cpp/faq/windows_timer_api.htm

			if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
				return;

			//if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
				//return;

			// All of them hardwork
			enemy.onFrame();
			macro.onFrame();
			strategy.onFrame();
			tech.onFrame();
			army.onFrame();

			debug.drawOnFrame();
			// End of them hardwork

			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> diffThisFrame = end - start;
			debug.displayBot.elapsedTime(diffThisFrame.count());

			std::chrono::duration<double, std::milli> diffLastFrame = end - lastFrame;
			lastFrame = end;
			debug.displayBroodwar.elapsedTime(diffLastFrame.count());

			// EXTRA set SPEED per KEEB
			if (Broodwar->getKeyState(K_1))
				Broodwar->setLocalSpeed(42);

			if (Broodwar->getKeyState(K_2))
				Broodwar->setLocalSpeed(21);

			if (Broodwar->getKeyState(K_3))
				Broodwar->setLocalSpeed(8);

			if (Broodwar->getKeyState(K_4))
				Broodwar->setLocalSpeed(1);

		} catch (const std::exception &e) {
			Broodwar << "EXCEPTION: " << e.what() << std::endl;
		}
	}

	void BlackCrow::onSendText(std::string text) {
		BWEM::utils::MapDrawer::ProcessCommand(text);

		if (!debug.command(text)) {
			Broodwar->sendText(text.c_str());
		}
	}

	void BlackCrow::onReceiveText(BWAPI::Player player, std::string text) {
		Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
	}

	void BlackCrow::onPlayerLeft(BWAPI::Player player) {
		Broodwar->sendText("Caw?!");
	}

	void BlackCrow::onNukeDetect(BWAPI::Position target) {
		if (target) {
			Broodwar << "Nuclear Launch Detected at " << target << std::endl;
		} else {
			Broodwar->sendText("Where's the nuke?");
		}
	}

	void BlackCrow::onUnitDiscover(BWAPI::Unit unit) {
		builder.onBuildingDetected(unit);
		enemy.enemyDiscovered(unit);

		//if (Broodwar->getFrameCount() != 0)
		//Broodwar->sendText("Unit %s with id %i discovered!", unit->getType().c_str(), unit->getID());
	}

	void BlackCrow::onUnitEvade(BWAPI::Unit unit) {
		//Broodwar->sendText("Unit %i evaded!", unit->getID());
	}

	void BlackCrow::onUnitShow(BWAPI::Unit unit) {
		//Broodwar->sendText("Unit %s (%i) showed!", unit->getType().c_str(), unit->getID());
	}

	void BlackCrow::onUnitHide(BWAPI::Unit unit) {
		//Broodwar->sendText("Unit %s (%i) hid!", unit->getType().c_str(), unit->getID());
	}

	void BlackCrow::onUnitCreate(BWAPI::Unit unit) {
		//if (Broodwar->getFrameCount() != 0)
			//Broodwar->sendText("Unit %s created!", unit->getType().c_str());

		army.onUnitCreated(unit);
	}

	void BlackCrow::onUnitDestroy(BWAPI::Unit unit) {
		try {
			// BWEM
			if (unit->getType().isMineralField())
				bwem.OnMineralDestroyed(unit);
			else if (unit->getType().isSpecialBuilding())
				bwem.OnStaticBuildingDestroyed(unit);

			// Other
			army.onUnitDestroyed(unit);
			builder.onBuildingDestroyed(unit);
			macro.onUnitDestroyed(unit);
			enemy.onUnitDestroyed(unit);

		} catch (const std::exception & e) {
			Broodwar << "EXCEPTION: " << e.what() << std::endl;
		}

		//Broodwar->sendText("Unit %s destroyed!", unit->getType().c_str());
	}

	void BlackCrow::onUnitMorph(BWAPI::Unit unit) {
		builder.onBuildingDetected(unit);
		//Broodwar->sendText("Unit %s morphed!", unit->getType().c_str());
	}

	void BlackCrow::onUnitRenegade(BWAPI::Unit unit) {
		army.onUnitDestroyed(unit);
		macro.onUnitDestroyed(unit);

		//Broodwar->sendText("Unit %s went renegade!", unit->getType().c_str());
	}

	void BlackCrow::onUnitComplete(BWAPI::Unit unit) {
		builder.onBuildingDetected(unit);
		macro.onUnitCompleted(unit);

		//if (Broodwar->getFrameCount() != 0)
		//Broodwar->sendText("Unit %s complete!", unit->getType().c_str());
	}

	void BlackCrow::onSaveGame(std::string gameName) {
		Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
	}

	void BlackCrow::onEnd(bool isWinner) {
		if (isWinner) {
			// Log your win here!
		}
	}

	void BlackCrow::init() {
		
		Broodwar->setLatCom(false);
		Broodwar->setCommandOptimizationLevel(2);
		//Broodwar->sendText("Caw caw caw!");
		//Broodwar->sendText(("LatCom is %s", Broodwar->isLatComEnabled() ? "enabled" : "disabled"));

		//Broodwar->setLocalSpeed(0);
		//Broodwar->setGUI(false);

		//if (Broodwar->isDebug()) {
		Broodwar->setLocalSpeed(config.localSpeed);
		Broodwar->enableFlag(Flag::UserInput);
		//}
	}

	bool BlackCrow::isExecutingCommandFrame() {
		if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() == 0)
			return true;

		return false;
	}
}