#define NOMINMAX
#include "Debug.h"
#include "BlackCrow.h"
#include "Macro.h"
#include "Squad.h"
#include "SquadUnit.h"
#include "EnemyUnit.h"
#include "Tech.h"
#include <BW/Offsets.h>


namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Debug::Debug(BlackCrow &parent) : bc(parent) {

		showBaseInfo = false;
		showBuildable = false;
		fastDrawBuildable = false;
		showManagerInfos = true;
		showPlacementInfos = true;
		showBwem = false;
		showSquadInfo = false;
		showEnemyUnits = true;
		showLifeBars = true;

		displayBroodwar.setBackgroundColor(Color(0, 100, 0)); // Dark Green
		displayBroodwar.setShowPercentage(105);
		displayBroodwar.setShowSpikes(false);

		plannedStatusStrings.emplace(Planned::Status::ACTIVE, "Active");
		plannedStatusStrings.emplace(Planned::Status::COMPLETED, "Completed");
		plannedStatusStrings.emplace(Planned::Status::FAILED, "Failed");
	}

	void Debug::onStart() {}

	bool Debug::command(std::string text) {

		if (text == "help") {
			Broodwar << "Commands are: ib (base info), im (manager info), mb (map building placement)" << std::endl;
			Broodwar << "pi (placement info), test (changes)" << std::endl;
			Broodwar << "bwem (bwem terrain data)" << std::endl;
			return true;
		}

		

		if (text == "1") {
			bc.enemy.enemies.clear();
			return true;
		}

		if (text == "2") {
			auto aaa = bc.tech.getTechPath(UnitTypes::Zerg_Mutalisk);
			Broodwar->sendText("Size is %i", aaa.size());

			for (auto item : aaa.items) {
				Broodwar->sendText(item.getName().c_str());
			}

			return true;
		}

		if (text == "ib") {
			showBaseInfo = !showBaseInfo;
			Broodwar->sendText(("Base information turned " + getOnOffString(showBaseInfo)).c_str());
			return true;
		}

		if (text == "buildable") {
			if (!showBuildable) {
				showBuildable = true;
				fastDrawBuildable = false;
				Broodwar->sendText(("Buildable turned " + getOnOffString(showBuildable)).c_str());
			} else {
				if (!fastDrawBuildable) {
					fastDrawBuildable = true;
					Broodwar->sendText(("Fast Draw for Buildable turned " + getOnOffString(fastDrawBuildable)).c_str());
				} else {
					showBuildable = false;
					Broodwar->sendText(("Buildable turned " + getOnOffString(showBuildable)).c_str());
				}
			}
			//Broodwar->sendText("Buildable turned %s, Fast Draw is %s", getOnOffString(showBuildable), getOnOffString(fastDrawBuildable));
			return true;
		}

		if (text == "manager") {
			showManagerInfos = !showManagerInfos;
			Broodwar->sendText(("Show Manager turned " + getOnOffString(showManagerInfos)).c_str());
			return true;
		}

		if (text == "placement") {
			showPlacementInfos = !showPlacementInfos;
			Broodwar->sendText(("Placement info turned " + getOnOffString(showPlacementInfos)).c_str());
			return true;
		}

		if (text == "bwem") {
			showBwem = !showBwem;
			Broodwar->sendText(("BWEM info turned " + getOnOffString(showBwem)).c_str());
			return true;
		}

		if (text == "squad") {
			showSquadInfo = !showSquadInfo;
			Broodwar->sendText(("Squad info turned " + getOnOffString(showSquadInfo)).c_str());
			return true;
		}

		if (text == "enemy") {
			showEnemyUnits = !showEnemyUnits;
			Broodwar->sendText(("Enemy units turned " + getOnOffString(showEnemyUnits)).c_str());
			return true;
		}

		if (text == "lifebars") {
			showLifeBars = !showLifeBars;
			Broodwar->sendText(("Lifebars turned " + getOnOffString(showLifeBars)).c_str());
			return true;
		}

		return false;
	}



	void Debug::drawOnFrame() {

		// Draw current order
		Broodwar->setTextSize(BWAPI::Text::Size::Small);
		for (auto unit : Broodwar->self()->getUnits()) {
			//Broodwar->drawTextMap(unit->getPosition(), "%s", unit->getOrder().c_str());
		}
		Broodwar->setTextSize(BWAPI::Text::Size::Default);

		// Selected Units Info
		int yOffset = 0;
		Color darkRed = Color(96, 0, 0);
		for (SquadUnitPtr sunit : bc.army.sunits) {
			EnemyUnitPtr eu = bc.enemy.getEnemy(sunit->attackTargetId);

			if (eu) {
				if (sunit->unit->isSelected()) {
					Unit u = Broodwar->getUnit(sunit->attackTargetId);
					if (u)
						Broodwar->drawTextScreen(200, 10 + yOffset++ * 15, "\x11 Target ID: %i, Type %s", sunit->attackTargetId, eu->type.getName().c_str());
					else
						Broodwar->drawTextScreen(200, 10 + yOffset++ * 15, "\x11 Target ID: %i", sunit->attackTargetId);
				}

				if (sunit->hasTarget())
					Broodwar->drawLineMap(sunit->unit->getPosition(), eu->position, darkRed);
			}
		}

		// Builder Debug
		//int i = 0;
		//for (BWAPI::TilePosition p : bc.builder.positions)
		//	Broodwar->drawTextMap(Position(p), "%i", i++);

		// Minerals
		//Broodwar->drawTextScreen(10, 65, "Average Smoothed Minerals: %f", bc.macro.getAverageMineralsPerFrame());
		//Broodwar->drawTextScreen(10, 50, "Average Minerals: %f", bc.macro.mineralFrameAverage);
		
		//Broodwar->drawTextScreen(10, 80, "Average Gas: %f", bc.macro.gasFrameAverage);
		//Broodwar->drawTextScreen(10, 95, "Average Smoothed Gas: %f", bc.macro.getAverageGasPerFrame());

		


		/* 
		// Unit Mix and Strategy calculation numbers
		int ox = 250;
		int yx = 250;

		Broodwar->drawTextScreen(ox, yx, "UMix s/pf: %f", bc.strategy.unitMix->supplyPerFrame());
		Broodwar->drawTextScreen(ox, yx + 10, "UMix m/pf: %f", bc.strategy.unitMix->mineralPerFrame());
		Broodwar->drawTextScreen(ox, yx + 20, "UMix l/pf: %f", bc.strategy.unitMix->larvaPerFrame());
		Broodwar->drawTextScreen(ox, yx + 30, "avg m/pf: %f", bc.macro.getAverageMineralsPerFrame());
		Broodwar->drawTextScreen(ox, yx + 40, "avg l/pr: %f", bc.macro.getAverageLarvaePerFrame());
		Broodwar->drawTextScreen(ox, yx + 50, "prod multi min: %f", bc.strategy.productionMultiplierMinerals);
		Broodwar->drawTextScreen(ox, yx + 60, "prod multi larvae: %f", bc.strategy.productionMultiplierLarvae);
		Broodwar->drawTextScreen(ox, yx + 70, "prod multi: %f", bc.strategy.productionMultiplier);
		*/

		// Draw Area information
		for (const AreaPtr& area : bc.map.areas) {
			auto center = (area->bwemArea.TopLeft() + area->bwemArea.BottomRight()) / 2;
			Broodwar->drawTextMap(Position(center), "%i enemy units", area->enemies.size());
		}

		// Draw APM, LSpeed, Time
		Broodwar->setTextSize(BWAPI::Text::Size::Small);
		Broodwar->drawTextScreen(120, 0, "APM: %i", Broodwar->getAPM());
		Broodwar->drawTextScreen(175, 0, "LSpeed: %i", BW::BWDATA::GameSpeedModifiers.gameSpeedModifiers[0]);
		int seconds = Broodwar->elapsedTime() % 60;
		if(seconds >= 10)
			Broodwar->drawTextScreen(230, 0, "Time: %i:%i", Broodwar->elapsedTime() / 60, seconds);
		else
			Broodwar->drawTextScreen(230, 0, "Time: %i:0%i", Broodwar->elapsedTime() / 60, seconds);
		Broodwar->setTextSize(BWAPI::Text::Size::Default);

		// Draw Mouse Tile Position
		//Broodwar->drawTextMouse(0, -10, "%i, %i", (Broodwar->getMousePosition().x + Broodwar->getScreenPosition().x) / 32, (Broodwar->getMousePosition().y + +Broodwar->getScreenPosition().y) / 32);

		if (showBaseInfo)
			drawBaseInformation();

		if (showBuildable)
			drawBuildable();

		if (showPlacementInfos)
			drawPlacementInfo();

		if (showBwem)
			drawBwem();

		if (showEnemyUnits)
			drawEnemyUnits();

		if (showLifeBars)
			drawLifeBars(); 
		
		if (showSquadInfo)
			drawSquadInfo();

		if (showManagerInfos)
			drawManagerInfo();

		drawFrameTimeDisplay();
	}

	void Debug::drawFrameTimeDisplay() {
		displayBot.updateAndDraw(0, 0);
		displayBroodwar.updateAndDraw(0, 10);
	}

	void Debug::drawBaseInformation() {
		// TODO
		for (Base& base : bc.macro.bases) {
			/*
			for (Mineral& mineral : base.minerals) {
				Broodwar->drawBoxMap(Position(mineral.bwemMineral->TopLeft()), Position(mineral.bwemMineral->TopLeft()) + Position(10, 10), Colors::Black, true);
				Broodwar->drawTextMap(Position(mineral.bwemMineral->TopLeft()) + Position(3, -1), std::to_string(mineral.workers.size()).c_str());
			}

			for (Geyser& geyser : base.geysers) {
				Broodwar->drawBoxMap(Position(geyser.bwemGeyser->TopLeft()), Position(geyser.bwemGeyser->TopLeft()) + Position(10, 10), Colors::Black);
				Broodwar->drawTextMap(Position(geyser.bwemGeyser->TopLeft()), std::to_string(geyser.workers.size()).c_str());
			}
			*/

			Broodwar->drawBoxMap(Position(base.bwemBase.Location()) + Position(25, 35), Position(base.bwemBase.Location()) + Position(100, 75), Colors::Black, true);
			Broodwar->drawTextMap(Position(base.bwemBase.Location()) + Position(30, 35), "Total: %i (+%i)", base.getTotalWorkers(), base.getWorkersNeeded());
			Broodwar->drawTextMap(Position(base.bwemBase.Location()) + Position(30, 45), "Mineral: %i", base.getTotalMineralWorkers());
			Broodwar->drawTextMap(Position(base.bwemBase.Location()) + Position(30, 55), "Gas: %i (+%i)", base.getTotalGasWorkers(), base.getGasWorkerSlotsAvailable());
		}
	}

	void Debug::drawBuildable() {
		for (int x = 0; x < Broodwar->mapWidth(); x++) {
			for (int y = 0; y < Broodwar->mapHeight(); y++) {

				bool draw = false;
				Color color = Colors::Orange;;

				if (!bc.map.mapTiles[x][y].resourceBuildable) {
					draw = true;
				}

				if (!bc.map.mapTiles[x][y].buildable) {
					draw = true;
					color = Colors::Red;
				}

				if (!bc.map.mapTiles[x][y].mineralLine) {
					draw = true;
					color = Colors::Yellow;
				}

				if (draw) {

					if (fastDrawBuildable)
						Broodwar->drawBoxMap(32 * x + 2, 32 * y + 2, 32 * x + 28, 32 * y + 28, color, false);
					else {
						// Note: Using Position classes costs additional 30% (at least in debug)
						int border = 1;
						int length = 5;

						// Top Left
						Broodwar->drawLineMap(x * 32 + border, y * 32 + border, x * 32 + border + length, y * 32 + border, color);
						Broodwar->drawLineMap(x * 32 + border, y * 32 + border, x * 32 + border, y * 32 + border + length, color);

						// Top Right
						Broodwar->drawLineMap(x * 32 + 32 - border, y * 32 + border, x * 32 + 32 - border - length, y * 32 + border, color);
						Broodwar->drawLineMap(x * 32 + 32 - border, y * 32 + border, x * 32 + 32 - border, y * 32 + border + length, color);

						// Bottom Left
						Broodwar->drawLineMap(x * 32 + border, y * 32 + 32 - border, x * 32 + border, y * 32 + 32 - border - length, color);
						Broodwar->drawLineMap(x * 32 + border, y * 32 + 32 - border, x * 32 + border + length, y * 32 + 32 - border, color);

						// Bottom Right
						Broodwar->drawLineMap(x * 32 + 33 - border, y * 32 + 32 - border, x * 32 + 32 - border - length, y * 32 + 32 - border, color);
						Broodwar->drawLineMap(x * 32 + 32 - border, y * 32 + 32 - border, x * 32 + 32 - border, y * 32 + 32 - border - length, color);
					}
				}
			}
		}
	}

	std::string Debug::getOnOffString(bool value) {
		return value ? "on" : "off";
	}

	std::string Debug::getShortName(std::string longName) {
		std::string::size_type pos = longName.find("_");

		if (pos != std::string::npos)
			return longName.substr(pos + 1);

		return longName;
	}

	void Debug::drawManagerInfo() {
		// Strategy Manager
		// Build Order

		/*
		if (bc.strategy.buildOrder.size() > 0) {
			int xStart = 500;
			int yStart = 20;

			Broodwar->drawTextScreen(xStart, yStart, "\x03 Build Order Items left %i", bc.strategy.buildOrder.size());
			Broodwar->drawLineScreen(xStart + 4, yStart + 15, xStart + 120, yStart + 15, BWAPI::Colors::Yellow);

			int offset = 0;
			for (auto item : bc.strategy.buildOrder) {
				Broodwar->drawTextScreen(xStart + 5, yStart + 17 + offset, item.c_str());
				offset += 10;
			}
		}*/

		{
			// Macro Manager
			// Planned Units
			int xStart = 20;
			int yStart = 20;

			Broodwar->drawTextScreen(xStart + 1, yStart, "Planned Units");
			Broodwar->drawLineScreen(xStart, yStart + 13, xStart + 120, yStart + 13, BWAPI::Colors::Yellow);

			int i = 1;
			for (std::shared_ptr<Planned> planned : bc.macro.plannedStuff) {
				float percentage = planned->progressPercent();
				if (percentage > 0)
					Broodwar->drawBoxScreen(xStart, yStart + 11 + 13 * i, xStart + (int)(120.0 * percentage), yStart + 14 + 13 * i, BWAPI::Colors::White, true);

				Broodwar->drawTextScreen(xStart + 1, yStart + 13 * i, planned->getName().c_str());

				std::shared_ptr<PlannedUnit> plannedUnit = std::dynamic_pointer_cast<PlannedUnit>(planned);
				//if (plannedUnit && plannedUnit->unit)
					//Broodwar->drawTextScreen(xStart - 22, yStart + 13 * i, std::to_string(plannedUnit->unit->getID()).c_str());
				
				i++;
			}
		}
	}


	// Plannet Building Locations
	void Debug::drawPlacementInfo() {
		// TODO
		/*
		for (PlannedUnit* pu : *bc.macro.plannedUnits) {
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
		*/
	}

	void Debug::drawBwem() {
		BWEM::utils::gridMapExample(bc.bwem);
		BWEM::utils::drawMap(bc.bwem);
	}

	void Debug::drawSquadInfo() {
		for (ScoutSquadPtr scoutSquad : bc.army.scoutSquads) {
			if (scoutSquad->sunits.size() > 0) {
				auto positionsIt = scoutSquad->getScoutingPositions().begin();

				while (positionsIt != scoutSquad->getScoutingPositions().end()) {
					BWAPI::Unit scoutUnit = scoutSquad->sunits.back()->unit;
					TilePosition& tilePosition = *positionsIt;

					if (*positionsIt == scoutSquad->getScoutingPositions().back()) {
						Broodwar->drawLineMap(scoutUnit->getPosition().x, scoutUnit->getPosition().y, tilePosition.x * 32, tilePosition.y * 32, BWAPI::Colors::White);
					} else {
						Broodwar->drawLineMap(scoutUnit->getPosition().x, scoutUnit->getPosition().y, tilePosition.x * 32, tilePosition.y * 32, BWAPI::Colors::Grey);
					}
					positionsIt++;
				}
			}
		}
	}

	void Debug::drawEnemyUnits() {
		Broodwar->setTextSize(BWAPI::Text::Size::Small);

		for (EnemyUnitPtr eu : bc.enemy.enemies) {
			Unit unit = Broodwar->getUnit(eu->id);
			bool a = unit->isVisible();

			if (!unit->isVisible()) {
				if (eu->type.isBuilding()) {
					Broodwar->drawBoxMap(eu->tilePosition.x * 32, eu->tilePosition.y * 32, eu->tilePosition.x * 32 + eu->type.tileWidth() * 32, eu->tilePosition.y * 32 + eu->type.tileHeight() * 32, Colors::Grey, false);
					Broodwar->drawTextMap(eu->tilePosition.x * 32, eu->tilePosition.y * 32, getShortName(eu->type.getName()).c_str());
				} else {
					Color borderColor = eu->isGhost ? Colors::Black : Colors::Grey;
					Broodwar->drawBoxMap(eu->position.x, eu->position.y, eu->position.x + eu->type.width(), eu->position.y + eu->type.height(), borderColor, false);
					Broodwar->drawTextMap(eu->position.x, eu->position.y, getShortName(eu->type.getName()).c_str());
				}
			}
		}

		Broodwar->setTextSize(BWAPI::Text::Size::Default);
	}

	void Debug::drawLifeBars() {
		float barSize = bc.config.barSize; // Pixel width and height of a single health rectangle

		for (Unit unit : Broodwar->getAllUnits()) {
			if (!unit->getPlayer()->isNeutral()) {
				int length = (int)(unit->getType().width() * 0.85);
				int bars = (int)(length / (barSize + 1));
				int pixelLength = (int)(bars * barSize + 2);

				Position barPos = unit->getPosition() - Position((int)(pixelLength * 0.5), (int)(unit->getType().dimensionUp() * 1.2));
				float percentage = (float)unit->getHitPoints() / (float)unit->getType().maxHitPoints();

				// TODO Colors are cacheable
				Util::HsvColor hsvColor;
				hsvColor.h = (char)(-(0xFF * 0.33) * percentage);
				hsvColor.s = 0xFF;
				hsvColor.v = (char)(0xFF * 0.7);

				Util::RgbColor rgbColor = Util::HsvToRgb(hsvColor);
				Color barColor(rgbColor.r, rgbColor.b, rgbColor.g);

				// Draw
				Broodwar->drawBoxMap(barPos, barPos + Position(pixelLength - 1, (int)barSize + 2), Colors::Black, true);
				for (int i = 0; i < bars; i++) {
					Color color = i < (float)bars * percentage ? barColor : Colors::Grey;
					int barSizePixel = (int)(barSize * i);

					Position topLeft(1 + barSizePixel, 1);
					Position bottomRight((int)barSize + barSizePixel, (int)barSize + 1);
					Broodwar->drawBoxMap(barPos + topLeft, barPos + bottomRight, color, true);
				}
			}
		}
	}

	void Debug::drawTestPath() {
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
		const BWEM::CPPath& path = bc.bwem.GetPath(Position(left), Position(right), &length);

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

	// ############# Debug Performance Display ###############
	void DebugPerformanceDisplay::elapsedTime(double millis) {
		frameTimeHistory.push_front(millis);

		if (highestFrameTime < millis) {
			highestFrameTime = millis;
			highestFrameTimeAgo = 0;
		}
	}

	void DebugPerformanceDisplay::setBackgroundColor(BWAPI::Color color) {
		backgroundColor = color;
	}

	void DebugPerformanceDisplay::updateAndDraw(int xStart, int yStart) {
		int logicalFrameSpeed = BW::BWDATA::GameSpeedModifiers.gameSpeedModifiers[0];

		// Don't show if speed is 0
		if (logicalFrameSpeed == 0)
			return;

		//Framerates
		if (frameTimeHistory.size() >= Broodwar->getFPS() * .5) {
			if (frameTimeHistory.size() > 0)
				frameTimeHistory.pop_back();
		}

		if (++highestFrameTimeAgo > Broodwar->getFPS() * 11) {
			highestFrameTime = 0;
			highestFrameTimeAgo = 0;
		}

		// Frame time taken
		if (frameTimeHistory.size() > 0) {
			double average = 0;
			for (double frameTime : frameTimeHistory) {
				average += frameTime;
			}
			average /= frameTimeHistory.size();

			Color blackBarColor = Colors::Black;
			bool overLimit = false;
			double percent = average / logicalFrameSpeed;
			double barPercent = percent;

			if (showPercentage / 100 > percent)
				return;

			// Current Load
			Broodwar->drawBoxScreen(xStart, yStart, xStart + 23, yStart + 9, backgroundColor, true);
			Broodwar->setTextSize(BWAPI::Text::Size::Small);

			if (percent > 1) {
				overLimit = true;
				barPercent = 1;
				blackBarColor = Colors::Red;
			}

			Broodwar->drawTextScreen(xStart + 1, yStart + -1, std::to_string((int)(percent * 100.0)).c_str());
			Broodwar->drawTextScreen(xStart + 15, yStart + -1, "%%");

			// Black Bar
			Broodwar->drawBoxScreen(xStart + 23, yStart + 0, xStart + 100, yStart + 9, blackBarColor, true);
			// Indicator Helper Bars
			for (int i = 0; i <= logicalFrameSpeed; i++) {
				int xPos = (int)((75.0 / logicalFrameSpeed) * i);
				if (i % 5 == 0) {
					Broodwar->drawLineScreen(xStart + 24 + xPos, yStart + 0, xStart + 24 + xPos, yStart + 9, Colors::Grey);
				} else {
					Broodwar->drawLineScreen(xStart + 24 + xPos, yStart + 0, xStart + 24 + xPos, yStart + 1, Colors::Grey);
					Broodwar->drawLineScreen(xStart + 24 + xPos, yStart + 8, xStart + 24 + xPos, yStart + 9, Colors::Grey);
				}
				//Broodwar->drawTextScreen(xStart + 24 + xPos, yStart + 10, std::to_string(i).c_str());
			}

			// Highest Frame Indicator
			int indicatorXPos = 24 + (int)(75 * (std::min(highestFrameTime / logicalFrameSpeed, 1.0)));
			Broodwar->drawLineScreen(xStart + indicatorXPos, yStart + 0, xStart + indicatorXPos, yStart + 9, Colors::Yellow);
			// White Average Bar
			Broodwar->drawBoxScreen(xStart + 24, yStart + 2, xStart + 25 + (int)(74 * barPercent), yStart + 7, Colors::White, false);

			// Over the limit red bar
			if (showSpikes && highestFrameTime > logicalFrameSpeed) {
				Broodwar->drawBoxScreen(xStart + 100, yStart + 0, xStart + 166, yStart + 9, Colors::Red, true);
				Broodwar->drawTextScreen(xStart + 103, yStart + -1, "%i ms spike!", (int)highestFrameTime);
			}

			Broodwar->setTextSize(BWAPI::Text::Size::Default);
		}
	}

	void DebugPerformanceDisplay::setShowPercentage(double percentage) {
		showPercentage = percentage;
	}

	void DebugPerformanceDisplay::setShowSpikes(bool show) {
		showSpikes = show;
	}
	// ############# Debug Performance Display - End ###############
}