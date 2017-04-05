#include "BlackCrow.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	BlackCrow::BlackCrow() : builder(*this), debug(*this), enemy(*this), macro(*this), map(*this), strategy(*this), bwem(BWEM::Map::Instance()) {}

	void BlackCrow::onStart() {

		try {

			auto start = std::chrono::high_resolution_clock::now();

			init();

			// BWEM
			bwem.Initialize();
			bwem.EnableAutomaticPathAnalysis();
			bool startingLocationsOK = bwem.FindBasesForStartingLocations();
			assert(startingLocationsOK);

			BWEM::utils::MapPrinter::Initialize(&bwem);
			//BWEM::utils::printMap(*map);      // will print the map into the file <StarCraftFolder>bwapi-data/map.bmp
			//BWEM::utils::pathExample(*map);   // add to the printed map a path between two starting locations
			// END of BWEM

			map.onStart();
			macro.onStart();
			strategy.onStart();
			enemy.onStart();
			builder.onStart();
			debug.onStart();

			auto end = std::chrono::high_resolution_clock::now();

			std::chrono::duration<double, std::milli> diff = end - start;
			Broodwar->sendText("Init has taken %fms", diff.count());
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

			debug.drawOnFrame();

			// Extra
			// Draw Mouse Tile Position
			//Broodwar->drawTextMouse(0, -10, "%i, %i", (Broodwar->getMousePosition().x + Broodwar->getScreenPosition().x) / 32, (Broodwar->getMousePosition().y + +Broodwar->getScreenPosition().y) / 32);

			if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
				return;

			if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
				return;

			// All of them hardwork
			enemy.onFrame();
			macro.onFrame();
			strategy.onFrame();
			// End of them hardwork

			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> diff = end - start;
			debug.elapsedMs = diff.count();

		} catch (const std::exception &e) {
			Broodwar << "EXCEPTION: " << e.what() << std::endl;
		}
	}

	void BlackCrow::testPath() {
		static Position left, right;

		Broodwar->drawCircleMap(left, 10, BWAPI::Colors::Red);
		Broodwar->drawCircleMap(right, 10, BWAPI::Colors::Red);

		// Testdistances
		if (Broodwar->getMouseState(BWAPI::MouseButton::M_LEFT)) {
			left = Broodwar->getMousePosition() + Broodwar->getScreenPosition();
		}

		if (Broodwar->getMouseState(BWAPI::MouseButton::M_RIGHT)) {
			right = Broodwar->getMousePosition() + Broodwar->getScreenPosition();
		}

		Broodwar->drawCircleMap(left, 15, BWAPI::Colors::Red);
		Broodwar->drawCircleMap(right, 15, BWAPI::Colors::Red);

		int length;
		const BWEM::CPPath& path = bwem.GetPath(Position(left), Position(right), &length);

		if (path.empty()) { // no ChokePoint between a and b: 
			// just draw a single line between them:
			Broodwar->drawLineMap(left, right, BWAPI::Colors::Cyan);
		} else {
			// draw a line between each ChokePoint in Path:
			const BWEM::ChokePoint * cpPrevious = nullptr;
			for (const BWEM::ChokePoint * cp : path) {
				if (cpPrevious)
					Broodwar->drawLineMap(Position(cpPrevious->Center()), Position(cp->Center()), BWAPI::Colors::Cyan);
				cpPrevious = cp;
			}

			Broodwar->drawLineMap(left, Position(path.front()->Center()), BWAPI::Colors::Cyan);
			Broodwar->drawLineMap(right, Position(path.back()->Center()), BWAPI::Colors::Cyan);
		}

		if (Broodwar->getMouseState(BWAPI::MouseButton::M_RIGHT) || Broodwar->getMouseState(BWAPI::MouseButton::M_RIGHT)) {
			Broodwar->sendText("BWEM length: %i", length);
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
		strategy.onUnitDiscovered(unit);
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
	}

	void BlackCrow::onUnitDestroy(BWAPI::Unit unit) {
		try {
			// BWEM
			if (unit->getType().isMineralField())
				bwem.OnMineralDestroyed(unit);
			else if (unit->getType().isSpecialBuilding())
				bwem.OnStaticBuildingDestroyed(unit);

			// Other
			builder.onBuildingDestroyed(unit);
			strategy.onUnitDestroyed(unit);
			macro.onUnitDestroyed(unit);

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
		Broodwar->sendText("Caw caw caw!");
		Broodwar->setCommandOptimizationLevel(2);

		//if (Broodwar->isDebug()) {
		Broodwar->setLocalSpeed(config.localSpeed);
		Broodwar->enableFlag(Flag::UserInput);
		//}
	}
}