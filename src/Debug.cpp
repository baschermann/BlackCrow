#include "Debug.h"

using namespace BWAPI;
using namespace Filter;
//namespace { BWEM::Map* map; }

Debug* debug = new Debug();

Debug::Debug() {
	showBaseInfo = false;
	showBuildable = true;
	showResourceBuildable = false;
	showManagerInfos = true;
	showPlacementInfos = true;
	showBwem = false;
	showSquadInfo = true;
	showEnemyUnits = true;
	showLifeBars = true;

	elapsedMs = 0;
	frameTimeHistory = new std::list<float>;
	highestFrameTime = 0;
	highestFrameTimeAgo = 0;
}

void Debug::onStart() {
	//map = &BWEM::Map::Instance();
}

bool Debug::command(std::string text) {

	if (text == "help") {
		Broodwar << "Commands are: ib (base info), im (manager info), mb (map building placement)" << std::endl;
		Broodwar << "pi (placement info), test (changes)" << std::endl;
		Broodwar << "bwem (bwem terrain data)" << std::endl;
		return true;
	}

	if (text == "test") {
		//Broodwar << macroManager->getNonReservedLarvae()->size() << std::endl;
		//Broodwar << macroManager->getNonReservedLarva((*macroManager->bases->begin())->hatchery->getPosition())->getID() << std::endl;
		macroManager->planBuilding(UnitTypes::Zerg_Hatchery, macroManager->firstBase->hatchery->getPosition(), macroManager->firstBase);
		return true;
	}

	if (text == "test2") {
		Broodwar << macroManager->reservedLarvae->size() << std::endl;
		//Broodwar << macroManager->getNonReservedLarva((*macroManager->bases->begin())->hatchery->getPosition())->getID() << std::endl;
		return true;
	}
	
	if (text == "ib") {
		showBaseInfo = !showBaseInfo;
		Broodwar->sendText(("Base information turned " + getOnOffString(showBaseInfo)).c_str());
		return true;
	}

	if (text == "buildable") {
		showBuildable = !showBuildable;
		showResourceBuildable = false;
		Broodwar->sendText(("Buildable turned " + getOnOffString(showBuildable)).c_str());
		return true;
	}

	if (text == "rbuildable") {
		showResourceBuildable = !showResourceBuildable;
		showBuildable = false;
		Broodwar->sendText(("Resource Buildable turned " + getOnOffString(showResourceBuildable)).c_str());
		return true;
	}

	if (text == "mi") {
		showManagerInfos = !showManagerInfos;
		Broodwar->sendText(("BWTA terrain turned " + getOnOffString(showManagerInfos)).c_str());
		return true;
	}

	if (text == "pi") {
		showPlacementInfos = !showPlacementInfos;
		Broodwar->sendText(("Placement info turned " + getOnOffString(showPlacementInfos)).c_str());
		return true;
	}

	if (text == "bwem") {
		showBwem = !showBwem;
		Broodwar->sendText(("BWEM info turned " + getOnOffString(showBwem)).c_str());
		return true;
	}

	if (text == "si") {
		showSquadInfo = !showSquadInfo;
		Broodwar->sendText(("Squad info turned " + getOnOffString(showSquadInfo)).c_str());
		return true;
	}

	if (text == "eu") {
		showEnemyUnits = !showEnemyUnits;
		Broodwar->sendText(("Enemy Units turned " + getOnOffString(showEnemyUnits)).c_str());
		return true;
	}

	if (text == "lb") {
		showLifeBars = !showLifeBars;
		Broodwar->sendText(("Enemy Units turned " + getOnOffString(showLifeBars)).c_str());
		return true;
	}

	return false;
}



void Debug::drawOnFrame() {

	BWAPI::Position prev;
	prev.x = 0;
	prev.y = 0;
	
	int i = 0;
	for (BWAPI::Position p : buildingManager->points) {
		//Broodwar->drawLineMap(prev.x, prev.y, p.x, p.y, Colors::Cyan);
		//prev = p;
		Broodwar->drawTextMap(p, std::to_string(i++).c_str());
	}

	if (showBaseInfo)
		drawBaseInformation();

	if (showBuildable)
		drawBuildable();

	if (showResourceBuildable)
		drawResourceBuildable();

	if (showManagerInfos)
		drawManagerInfo();

	if (showPlacementInfos)
		drawPlacementInfo();

	if (showBwem)
		drawBwem();

	if (showSquadInfo)
		drawSquadInfo();

	if (showEnemyUnits)
		drawEnemyUnits();

	if (showLifeBars)
		drawLifeBars();

	drawFrameTimeDisplay();
}

void Debug::drawFrameTimeDisplay() {

	// Get Current Logical Speed
	// Read from BW::BWDATA::GameSpeedModifiers.gameSpeedModifiers[0]; the local game speed, see Additional Setup.txt in the Project Folder.
	
	double logicalFrameSpeed = 6; // BW::BWDATA::GameSpeedModifiers.gameSpeedModifiers[0];

	//Framerates
	if (frameTimeHistory->size() >= ((float)Broodwar->getFPS() * .5)) {
		if (frameTimeHistory->size() > 0)
			frameTimeHistory->pop_back();
	}
	frameTimeHistory->push_front(elapsedMs);

	if (++highestFrameTimeAgo > Broodwar->getFPS() * 11) {
		highestFrameTime = 0;
		highestFrameTimeAgo = 0;
	}

	if (highestFrameTime < elapsedMs) {
		highestFrameTime = elapsedMs;
		highestFrameTimeAgo = 0;
	}

	// Frame time taken
	if (frameTimeHistory->size() > 0) {
		double average = 0;
		for (double frameTime : *frameTimeHistory) {
			average += frameTime;
		}
		average /= frameTimeHistory->size();

		// Current Load
		Broodwar->drawBoxScreen(0, 0, 23, 9, Colors::Brown, true);
		Broodwar->setTextSize(BWAPI::Text::Size::Small);

		Color blackBarColor = Colors::Black;
		bool overLimit = false;
		double percent = average / logicalFrameSpeed;
		double barPercent = percent;
		
		if (percent > 1) {
			overLimit = true;
			barPercent = 1;
			blackBarColor = Colors::Red;
		}

		Broodwar->drawTextScreen(1, -1, std::to_string((int)(percent * 100.0)).c_str());
		Broodwar->drawTextScreen(15, -1, "%%");

		// Black Bar
		Broodwar->drawBoxScreen(23, 0, 100, 9, blackBarColor, true);
		// Indicator Helper Bars
		for (int i = 0; i <= logicalFrameSpeed; i++) {
			int xPos = (75.0 / (int)logicalFrameSpeed) * i;
			if (i % 5 == 0) {
				Broodwar->drawLineScreen(24 + xPos, 0, 24 + xPos, 9, Colors::Grey);
			} else {
				Broodwar->drawLineScreen(24 + xPos, 0, 24 + xPos, 1, Colors::Grey);
				Broodwar->drawLineScreen(24 + xPos, 8, 24 + xPos, 9, Colors::Grey);
			}
			//Broodwar->drawTextScreen(24 + xPos, 10, std::to_string(i).c_str());
		}

		// Highest Frame Indicator
		int indicatorXPos = 24 + (75 * (std::min(highestFrameTime / logicalFrameSpeed, 1.0)));
		Broodwar->drawLineScreen(indicatorXPos, 0, indicatorXPos, 9, Colors::Yellow);
		// White Average Bar
		Broodwar->drawBoxScreen(24, 2, 25 + (74 * barPercent), 7, Colors::White, false);

		// Over the limit red bar
		if (highestFrameTime > logicalFrameSpeed) {
			Broodwar->drawBoxScreen(100, 0, 166, 9, Colors::Red, true);
			Broodwar->drawTextScreen(103, -1, "%i ms spike!", (int)highestFrameTime);
		}

		Broodwar->setTextSize(BWAPI::Text::Size::Default);
	}
}

void Debug::drawBaseInformation() {
	std::list<BaseInformation*> *allBaseInformation = macroManager->getAllBaseInformation();

		for (BaseInformation *baseInformation : *allBaseInformation) {
			int x = baseInformation->base->Center().x - 55;
			int y = baseInformation->base->Center().y - 20;

			Broodwar->drawBoxMap(x - 3, y, x + 114, y + 43, BWAPI::Colors::Black, true);
			//Broodwar->drawTextMap(x, y, "Position: %i, %i", x, y);

			Broodwar->drawTextMap(x, y, "isExpanding: %i", (int)baseInformation->isExpanding);

			int totalMinerals = 0;
			for (BWEM::Mineral* mineral : baseInformation->base->Minerals()) {
				totalMinerals += mineral->Amount();
			}
			Broodwar->drawTextMap(x, y + 10, "Minerals left: %i", totalMinerals);

			int totalGas = 0;
			for (BWEM::Geyser* geyser : baseInformation->base->Geysers()) {
				totalGas += geyser->Amount();
			}
			Broodwar->drawTextMap(x, y + 20, "Vespin-Gas left: %i", totalGas);
			Broodwar->drawTextMap(x, y + 30, "M: %i, Gas: %i", baseInformation->workersOnMinerals.size(), baseInformation->workersOnGas.size());
		}
}

void Debug::drawBuildable() {
	for (int x = 0; x < Broodwar->mapWidth(); x++) {
		for (int y = 0; y < Broodwar->mapHeight(); y++) {
			if (mapdata->mapTiles[x][y].buildable) {
				//Broodwar->drawBoxMap(32 * x + 2, 32 * y + 2, 32 * x + 28, 32 * y + 28, Colors::Grey, false);
			} else {
				Broodwar->drawBoxMap(32 * x + 2, 32 * y + 2, 32 * x +28 , 32 * y + 28, Colors::Red, false);
			}
		}
	}
}

void Debug::drawResourceBuildable() {

	Color* colorGreen = &Color(0, 255, 0);
	Color* colorRed = &Color(255, 0, 0);

	for (int x = 0; x < Broodwar->mapWidth(); x++) {
		for (int y = 0; y < Broodwar->mapHeight(); y++) {
			if (mapdata->mapTiles[x][y].resourceBuildable) {
				//Broodwar->drawBoxMap(32 * x + 2, 32 * y + 2, 32 * x + 28, 32 * y + 28, *colorGreen, false);
			} else {
				Broodwar->drawBoxMap(32 * x + 2, 32 * y + 2, 32 * x + 28, 32 * y + 28, *colorRed, false);
			}
		}
	}
}

std::string Debug::getOnOffString(bool value) {
	return value ? "on" : "off";
}


void Debug::drawManagerInfo() {
	// Strategy Manager
	// Build Order

	if(strategyManager->currentBuildOrder->size() > 0) {
		int xStart = 20;
		int yStart = 20;

		Broodwar->drawTextScreen(xStart, yStart, "\x03 Build Order");
		Broodwar->drawLineScreen(xStart, yStart + 15, xStart + 120, yStart + 15, BWAPI::Colors::Yellow);

		int i = 1;
		for (StrategyManager::BuildOrderItem* boi : *strategyManager->currentBuildOrder) {
			Broodwar->drawTextScreen(xStart, yStart + 5 + 9 * i, std::to_string(i).c_str());
			Broodwar->drawTextScreen(xStart + 10, yStart + 5 + 9 * i, boi->type.c_str());
			i++;
		}
	}

	{
		// Macro Manager
		// Planned Units
		int xStart = 500;
		int yStart = 20;
		
		Broodwar->drawTextScreen(xStart + 8, yStart, "L/D");
		Broodwar->drawTextScreen(xStart + 33, yStart, "Planned Units");
		Broodwar->drawLineScreen(xStart, yStart + 15, xStart + 120, yStart + 15, BWAPI::Colors::Yellow);

		int i = 1;
		for (PlannedUnit* pu : *macroManager->plannedUnits) {
			//Broodwar->drawTextScreen(xStart, yStart + 5 + 9 * i, std::to_string(i).c_str());
			//Broodwar->drawTextScreen(xStart + 8, yStart + 5 + 9 * i, pu->larvaEgg == nullptr ? " No" : "Yes");
			Broodwar->drawTextScreen(xStart, yStart + 5 + 13 * i, pu->larvaEgg == nullptr ? "-" : std::to_string(pu->larvaEgg->getID()).c_str());
			Broodwar->drawTextScreen(xStart + 27, yStart + 5 + 13 * i, pu->type.c_str());

			if (pu->larvaEgg) {
				float percentageFinished = (float) pu->larvaEgg->getRemainingBuildTime() / (float) pu->type.buildTime();
				Broodwar->drawLineScreen(xStart, yStart + 6 + 13 * i, xStart + (120.0 * percentageFinished), yStart + 6 + 13 * i, BWAPI::Colors::White);
				Broodwar->drawLineScreen(xStart, yStart + 17 + 13 * i, xStart + (120.0 * percentageFinished), yStart + 17 + 13 * i, BWAPI::Colors::White);
			}

			i++;
		}
	}
}


// Plannet Building Locations
void Debug::drawPlacementInfo() {
	for (PlannedUnit* pu : *macroManager->plannedUnits) {
		if (pu->type.isBuilding()) {
			if (pu->buildLocation) {
				Broodwar->drawBoxMap(pu->buildLocation.x * 32, pu->buildLocation.y * 32, pu->buildLocation.x * 32 + pu->type.tileWidth() * 32, pu->buildLocation.y * 32 + pu->type.tileHeight() * 32, BWAPI::Colors::Orange, false);
				if (pu->larvaEgg && pu->larvaEgg->getType() == BWAPI::UnitTypes::Zerg_Drone) {
					Broodwar->drawLineMap(pu->larvaEgg->getPosition().x, pu->larvaEgg->getPosition().y, pu->buildLocation.x * 32, pu->buildLocation.y * 32, BWAPI::Colors::Orange);
					//Broodwar->sendText("Build expansion at %i, %i", pu->buildLocation.x * 32, pu->buildLocation.y * 32);
				}
			}
		}
	}
}

void Debug::drawBwem() {
	BWEM::utils::gridMapExample(BWEM::Map::Instance());
	BWEM::utils::drawMap(BWEM::Map::Instance());
}

void Debug::drawSquadInfo() {
	for (ScoutSquad* ss : *strategyManager->scoutSquads) {
		for (BWAPI::TilePosition tilePosition : *ss->getScoutingPositions()) {
			BWAPI::Unit scoutUnit = (*ss->units.begin())->unit;
			Broodwar->drawLineMap(scoutUnit->getPosition().x, scoutUnit->getPosition().y, tilePosition.x * 32, tilePosition.y * 32, BWAPI::Colors::White);
		}
	}
}

void Debug::drawEnemyUnits() {
	for (EnemyUnit* eu : *enemy->enemies) {
		Unit unit = Broodwar->getUnit(eu->id);
		bool a = unit->isVisible();

		if (!unit->isVisible()) {
			if (eu->type.isBuilding()) {
				Broodwar->drawBoxMap(eu->tilePosition.x * 32, eu->tilePosition.y * 32, eu->tilePosition.x * 32 + eu->type.tileWidth() * 32, eu->tilePosition.y * 32 + eu->type.tileHeight() * 32, Colors::Grey, false);
				Broodwar->drawTextMap(eu->tilePosition.x * 32, eu->tilePosition.y * 32, eu->type.c_str());
			} else {
				Broodwar->drawBoxMap(eu->position.x, eu->position.y, eu->position.x + eu->type.width(), eu->position.y + eu->type.height(), Colors::Grey, false);
				Broodwar->drawTextMap(eu->position.x, eu->position.y, eu->type.c_str());
			}
		}
	}
}

void Debug::drawLifeBars() {
	float barSize = config->barSize;

	for (Unit unit : Broodwar->getAllUnits()) {
		if (!unit->getPlayer()->isNeutral()) {
			int length = ((float)unit->getType().width()) * 0.85;
			int bars = ((float)length / (barSize + 1));
			int pixelLength = bars * barSize + 2;

			Position barPos;
			barPos.x = unit->getPosition().x - ((float)pixelLength * 0.5);
			barPos.y = unit->getPosition().y - unit->getType().dimensionUp() * 1.2;

			float percentage = (float)unit->getHitPoints() / (float)unit->getType().maxHitPoints();

			// Colors are cacheable
			Util::HsvColor hsvColor;
			hsvColor.h = -(0xFF * 0.33) * percentage;
			hsvColor.s = 0xFF;
			hsvColor.v = 0xFF * 0.7;

			Util::RgbColor rgbColor = util->HsvToRgb(hsvColor);
			Color barColor(rgbColor.r, rgbColor.b, rgbColor.g);

			// Draw
			Broodwar->drawBoxMap(barPos.x, barPos.y, barPos.x + pixelLength - 1, barPos.y + barSize + 2, Colors::Black, true);
			for (int i = 0; i < bars; i++) {
				if (i < (float)bars * percentage)
					Broodwar->drawBoxMap(barPos.x + 1 + barSize* i, barPos.y + 1, barPos.x + barSize + barSize * i, barPos.y + barSize + 1, barColor, true);
				else
					Broodwar->drawBoxMap(barPos.x + 1 + barSize * i, barPos.y + 1, barPos.x + barSize + barSize * i, barPos.y + barSize + 1, Colors::Grey, true);
			}
		}
	}
}