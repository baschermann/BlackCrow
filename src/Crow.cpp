#include "Crow.h"

using namespace BWAPI;
using namespace Filter;
namespace { BWEM::Map* map = &BWEM::Map::Instance(); }


Crow::Crow() {
}

Crow::~Crow() {};

void Crow::onStart() {

	try {

		auto start = std::chrono::high_resolution_clock::now();

		init();

		// BWEM
		map->Initialize();
		map->EnableAutomaticPathAnalysis();
		bool startingLocationsOK = map->FindBasesForStartingLocations();
		assert(startingLocationsOK);

		BWEM::utils::MapPrinter::Initialize(map);
		//BWEM::utils::printMap(*map);      // will print the map into the file <StarCraftFolder>bwapi-data/map.bmp
		//BWEM::utils::pathExample(*map);   // add to the printed map a path between two starting locations
		// END of BWEM

		mapdata->onStart();
		macroManager->onStart();
		strategyManager->onStart();
		enemy->onStart();
		debug->onStart();
		buildingManager->onStart();

		auto end = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double, std::milli> diff = end - start;
		Broodwar->sendText("Init has taken %fms", diff);
		//Broodwar->sendText("black sheep wall");

	} catch (const std::exception &e) {
		Broodwar << "EXCEPTION: " << e.what() << std::endl;
	}
}

void Crow::onFrame() {
	try {
		auto start = std::chrono::high_resolution_clock::now();

		// Performance Qeuery Timer
		//http://www.decompile.com/cpp/faq/windows_timer_api.htm

		debug->drawOnFrame();

		// Extra
		// Draw Mouse Tile Position
		Broodwar->drawTextMouse(0, -10, "%i, %i", (Broodwar->getMousePosition().x + Broodwar->getScreenPosition().x) / 32, (Broodwar->getMousePosition().y + +Broodwar->getScreenPosition().y) / 32);


		if ( Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self() )
			return;

		if ( Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0 )
			return;

		// All of them hardwork
		enemy->onFrame();
		macroManager->update();
		strategyManager->update();
		// End of them hardwork

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> diff = end - start;
		debug->elapsedMs = diff.count();
		
	} catch (const std::exception &e) {
		Broodwar << "EXCEPTION: " << e.what() << std::endl;
	}
}

Position left;
Position right;

void Crow::test() {
	Broodwar->drawCircleMap(left, 10, BWAPI::Colors::Red);
	Broodwar->drawCircleMap(right, 10, BWAPI::Colors::Red);

	// Testdistances
	if (Broodwar->getMouseState(BWAPI::MouseButton::M_LEFT)) {
		left.x = (Broodwar->getMousePosition().x + Broodwar->getScreenPosition().x);
		left.y = (Broodwar->getMousePosition().y + Broodwar->getScreenPosition().y);
	}

	if (Broodwar->getMouseState(BWAPI::MouseButton::M_RIGHT)) {
		right.x = (Broodwar->getMousePosition().x + Broodwar->getScreenPosition().x);
		right.y = (Broodwar->getMousePosition().y + Broodwar->getScreenPosition().y);
	}

	Broodwar->drawCircleMap(left, 15, BWAPI::Colors::Red);
	Broodwar->drawCircleMap(right, 15, BWAPI::Colors::Red);

	int length;
	const BWEM::CPPath& path = map->GetPath(Position(left), Position(right), &length);

	if (path.empty()) { // no ChokePoint between a and b: 
		// just draw a single line between them:
		Broodwar->drawLineMap(left, right, BWAPI::Colors::Cyan);
	} else {
		// draw a line between each ChokePoint in Path:
		const BWEM::ChokePoint * cpPrevious = nullptr;
		for (const BWEM::ChokePoint * cp : path)
		{
			if (cpPrevious)	
				Broodwar->drawLineMap(cpPrevious->Center().x * 8, cpPrevious->Center().y * 8, cp->Center().x * 8, cp->Center().y * 8, BWAPI::Colors::Cyan);
			cpPrevious = cp;
		}

		Broodwar->drawLineMap(left.x, left.y, path.front()->Center().x * 8, path.front()->Center().y * 8, BWAPI::Colors::Cyan);
		Broodwar->drawLineMap(right.x, right.y, path.back()->Center().x * 8, path.back()->Center().y * 8, BWAPI::Colors::Cyan);
	}

	Broodwar->sendText("BWEM length: %i", length);
}

void Crow::onSendText(std::string text) {
	BWEM::utils::MapDrawer::ProcessCommand(text);

	if (!debug->command(text)) {
		Broodwar->sendText(text.c_str());
	}
}

void Crow::onReceiveText(BWAPI::Player player, std::string text) {
	Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void Crow::onPlayerLeft(BWAPI::Player player) {
	Broodwar->sendText("Caw?!");
}

void Crow::onNukeDetect(BWAPI::Position target) {
	if ( target ) {
		Broodwar << "Nuclear Launch Detected at " << target << std::endl;
	} else {
		Broodwar->sendText("Where's the nuke?");
	}
}

void Crow::onUnitDiscover(BWAPI::Unit unit) {
	buildingManager->onBuildingDetected(unit);
	strategyManager->onUnitDiscovered(unit);

	//if (Broodwar->getFrameCount() != 0)
		//Broodwar->sendText("Unit %s with id %i discovered!", unit->getType().c_str(), unit->getID());
}

void Crow::onUnitEvade(BWAPI::Unit unit) {
	//Broodwar->sendText("Unit %i evaded!", unit->getID());
}

void Crow::onUnitShow(BWAPI::Unit unit) {
	//Broodwar->sendText("Unit %s (%i) showed!", unit->getType().c_str(), unit->getID());
}

void Crow::onUnitHide(BWAPI::Unit unit) {
	//Broodwar->sendText("Unit %s (%i) hid!", unit->getType().c_str(), unit->getID());
}

void Crow::onUnitCreate(BWAPI::Unit unit) {
	//if (Broodwar->getFrameCount() != 0)
		//Broodwar->sendText("Unit %s created!", unit->getType().c_str());
}

void Crow::onUnitDestroy(BWAPI::Unit unit) {
	try {
		// BWEM
		if (unit->getType().isMineralField())
			map->OnMineralDestroyed(unit);
		else if (unit->getType().isSpecialBuilding())
			map->OnStaticBuildingDestroyed(unit);

		// Other
		buildingManager->onBuildingDestroyed(unit);
		strategyManager->onUnitDestroyed(unit);

	} catch (const std::exception & e) {
		Broodwar << "EXCEPTION: " << e.what() << std::endl;
	}

	//Broodwar->sendText("Unit %s destroyed!", unit->getType().c_str());
}

void Crow::onUnitMorph(BWAPI::Unit unit) {
	buildingManager->onBuildingDetected(unit);
	//Broodwar->sendText("Unit %s morphed!", unit->getType().c_str());
}

void Crow::onUnitRenegade(BWAPI::Unit unit) {
	//Broodwar->sendText("Unit %s went renegade!", unit->getType().c_str());
}

void Crow::onUnitComplete(BWAPI::Unit unit) {
	buildingManager->onBuildingDetected(unit);
	macroManager->onUnitComplete(unit);

	//if (Broodwar->getFrameCount() != 0)
		//Broodwar->sendText("Unit %s complete!", unit->getType().c_str());
}

void Crow::onSaveGame(std::string gameName) {
  Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void Crow::onEnd(bool isWinner) {
	if (isWinner) {
		// Log your win here!
	}
}

void Crow::init() {
	Broodwar->sendText("Caw caw caw!");
	Broodwar->setCommandOptimizationLevel(2);

	//if (Broodwar->isDebug()) {
		Broodwar->setLocalSpeed(config->localSpeed);
		Broodwar->enableFlag(Flag::UserInput);
	//}
}