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
		displayBroodwar.setBackgroundColor(Color(0, 100, 0)); // Dark Green
		displayBroodwar.setShowPercentage(105);
		displayBroodwar.setShowSpikes(false);
	}

	void Debug::onStart() {

		bc.debug.runOnEveryCell([](const unsigned int x, const unsigned int y, const Map::Cell& cell) {
			if(cell.enemiesInRange.size() > 0)
				Broodwar->drawTextMap(Position(x * 32 + 12, y * 32 + 12), std::to_string(cell.enemiesInRange.size()).c_str());
		});



		addCommand("help", "Shows all chat commands with description", false, true, [&bc = bc]() {
			for (auto command : bc.debug.commands) {
				Broodwar->sendText("%s -> %s", command.command.c_str(), command.description.c_str());
			}
		});
		
		addCommand("test", "Temporary Test", false, true, []() {
			Broodwar->sendText("Test output");
		});

		addCommand("bases", "Show base information", false, false, [&bc = bc]() {
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
		});

		addCommand("buildable", "Show buildable space", false, false, [&bc = bc]() {
			for (int x = 0; x < Broodwar->mapWidth(); x++) {
				for (int y = 0; y < Broodwar->mapHeight(); y++) {

					bool draw = false;
					Color color = Colors::Orange;;

					if (!bc.map.tiles[x][y].resourceBuildable) {
						draw = true;
					}

					if (!bc.map.tiles[x][y].buildable) {
						draw = true;
						color = Colors::Red;
					}

					if (!bc.map.tiles[x][y].mineralLine) {
						draw = true;
						color = Colors::Yellow;
					}

					if (draw) {

						//if (fastDrawBuildable)
							Broodwar->drawBoxMap(32 * x + 2, 32 * y + 2, 32 * x + 28, 32 * y + 28, color, false);
						/*else {
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
						//}
						*/
					}
				}
			}
		});

		addCommand("manager", "showManagerInfos", true, true, [&bc = bc]() {
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
		});

		addCommand("bwem", "Draws standard BWEM debug output", false, false, [&bc = bc]() {
			BWEM::utils::gridMapExample(bc.bwem);
			BWEM::utils::drawMap(bc.bwem);
		});

		addCommand("squad", "Shows squad information", true, false, [&bc = bc]() {
			for (SquadPtr squad : bc.army.squads) {
				if (squad->sunits.size() > 0) {
					for (auto& sunit : squad->sunits) {
						if (sunit->squadOverride == SquadUnit::Override::SCOUTING) {
							Broodwar->drawLineMap(sunit->self->getPosition(), Position(sunit->scoutLocation), BWAPI::Colors::White);
							Broodwar->drawBoxMap(Position(sunit->scoutLocation), Position(sunit->scoutLocation + TilePosition(1, 1) / 2), Colors::Green);
						}

						//Broodwar->drawCircleMap(sunit->self->getPosition(), 3, Colors::Green, true);
						if (sunit->squadOverride != SquadUnit::Override::NONE)
							Broodwar->drawTextMap(sunit->self->getPosition(), "OVR");

						//if(sunit->squad && sunit->squad->squadGoalTarget)
							//Broodwar->drawLineMap(sunit->self->getPosition(), sunit->squad->squadGoalTarget->position, Colors::Brown);
					}

					Broodwar->drawTextScreen(10, 120, "Squad unit amount: %i", squad->size());
				}
			}
		});

		addCommand("enemies", "Show enemy information", true, false, [&bc = bc]() {
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
		});

		addCommand("lifebars", "Draws life bars", true, false, [&bc = bc]() {
			float barSize = 3; // Pixel width and height of a single health/shield rectangle

			for (Unit unit : Broodwar->getAllUnits()) {
				if (!unit->getPlayer()->isNeutral()) {
					int length = (int)(unit->getType().width() * 0.85);
					int bars = (int)(length / (barSize + 1));
					int pixelLength = (int)(bars * barSize + 2);

					Position barPosHealth = unit->getPosition() - Position((int)(pixelLength * 0.5), (int)(unit->getType().dimensionUp() * 1.2));
					Position barPosShield = barPosHealth - Position(0, (int)barSize + 1);
					float percentageHealth = (float)unit->getHitPoints() / (float)unit->getType().maxHitPoints();
					float percentageShield = 0;

					bool hasShields = unit->getType().maxShields() > 0 ? true : false;
					if (hasShields)
						percentageShield = ((float)unit->getShields()) / (float)unit->getType().maxShields();

					// TODO Colors are cacheable
					Util::HsvColor hsvColor;
					hsvColor.h = (char)(-(0xFF * 0.33) * percentageHealth);
					hsvColor.s = 0xFF;
					hsvColor.v = (char)(0xFF * 0.7);

					Util::RgbColor rgbColor = Util::HsvToRgb(hsvColor);
					Color healthBarColor(rgbColor.r, rgbColor.b, rgbColor.g);

					// Draw Health
					Broodwar->drawBoxMap(barPosHealth, barPosHealth + Position(pixelLength - 1, (int)barSize + 2), Colors::Black, true);
					for (int i = 0; i < bars; i++) {
						Color color = i < (float)bars * percentageHealth ? healthBarColor : Colors::Grey;
						int barSizePixel = (int)(barSize * i);

						Position topLeft(1 + barSizePixel, 1);
						Position bottomRight((int)barSize + barSizePixel, (int)barSize + 1);
						Broodwar->drawBoxMap(barPosHealth + topLeft, barPosHealth + bottomRight, color, true);
					}

					if (hasShields) { // TODO Not 100% correct
						Broodwar->drawBoxMap(barPosShield, barPosShield + Position(pixelLength - 1, (int)barSize + 2), Colors::Black, true);
						for (int i = 0; i < bars; i++) {
							Color color = i + 1 < ((float)bars + 1) * percentageShield ? Colors::Blue : Colors::Grey;
							int barSizePixel = (int)(barSize * i);

							Position topLeft(1 + barSizePixel, 1);
							Position bottomRight((int)barSize + barSizePixel, (int)barSize + 1);
							Broodwar->drawBoxMap(barPosShield + topLeft, barPosShield + bottomRight, color, true);
						}
					}
				}
			}
		});

		addCommand("bwempath", "Draws a BWEM test path with left & right mouse click", false, false, [&bc = bc]() {
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
		});

		addCommand("area", "Shows area information", true, false, [&bc = bc]() {
			for (const AreaPtr& area : bc.map.areas) {
				auto center = (area->bwemArea.TopLeft() + area->bwemArea.BottomRight()) / 2;
				Broodwar->drawTextMap(Position(center), "%i enemy units", area->enemies.size());
			}
		});

		addCommand("network", "Shows latency, turn size etc.", true, false, [&bc = bc]() {
			// Draw APM, LSpeed, Time
			Broodwar->setTextSize(BWAPI::Text::Size::Small);
			Broodwar->drawTextScreen(120, 0, "APM: %i", Broodwar->getAPM());
			Broodwar->drawTextScreen(185, 0, "LS: %.1f", bc.getAverageFrameTime());
			int seconds = Broodwar->elapsedTime() % 60;
			if (seconds >= 10)
				Broodwar->drawTextScreen(240, 0, "Time: %i:%i", Broodwar->elapsedTime() / 60, seconds);
			else
				Broodwar->drawTextScreen(240, 0, "Time: %i:0%i", Broodwar->elapsedTime() / 60, seconds);
			Broodwar->setTextSize(BWAPI::Text::Size::Default);

			// Latency
			Broodwar->setTextSize(BWAPI::Text::Size::Small);
			Broodwar->drawTextScreen(120, 10, "LF: %i", Broodwar->getLatencyFrames());
			Broodwar->drawTextScreen(185, 10, "L: %i", Broodwar->getLatency());
			Broodwar->drawTextScreen(240, 10, "RLF: %i", Broodwar->getRemainingLatencyFrames());
			Broodwar->drawTextScreen(305, 10, "TS: %i", Broodwar->getLatencyFrames() - Broodwar->getLatency() + 1);
			Broodwar->setTextSize(BWAPI::Text::Size::Default);
		});

		addCommand("performance", "Shows top left performance counter", true, false, [&]() {
			displayBot.updateAndDraw(bc, 0, 0);
			//displayBroodwar.updateAndDraw(bc, 0, 10); // TODO fix, broke with taking the average of frame time
		});

		addCommand("unitmix", "Unit Mix information", false, false, [&bc = bc]() {
			// Unit Mix and Strategy calculation numbers
			int ox = 250;
			int yx = 250;

			Broodwar->drawTextScreen(ox, yx, "UMix s/pf: %f", bc.strategy.unitMix->supplyPerFrame());
			Broodwar->drawTextScreen(ox, yx + 10, "UMix m/pf: %f", bc.strategy.unitMix->mineralPerFrame());
			Broodwar->drawTextScreen(ox, yx + 20, "UMix l/pf: %f", bc.strategy.unitMix->larvaPerFrame());
			Broodwar->drawTextScreen(ox, yx + 30, "avg m/pf: %f", bc.macro.getAverageMineralsPerFrame());
			Broodwar->drawTextScreen(ox + 100, yx + 30, "avger m/pf: %f", bc.macro.mineralIncomeAverager.average);
			Broodwar->drawTextScreen(ox, yx + 40, "avg l/pr: %f", bc.macro.getAverageLarvaePerFrame());
			Broodwar->drawTextScreen(ox, yx + 50, "prod multi min: %f", bc.strategy.productionMultiplierMinerals);
			Broodwar->drawTextScreen(ox, yx + 60, "prod multi larvae: %f", bc.strategy.productionMultiplierLarvae);
			Broodwar->drawTextScreen(ox, yx + 70, "prod multi: %f", bc.strategy.productionMultiplier);
		});
		
		addCommand("testpath", "Draws pathfinding path", false, false, [&bc = bc]() {
			static PairUint left(0, 0);
			static PairUint right(0, 0);
			static PathResultUptr path = nullptr;

			if (Broodwar->getMouseState(MouseButton::M_LEFT)) {
				Position mouse = Broodwar->getMousePosition();
				Position screen = Broodwar->getScreenPosition();
				left.first = (mouse.x + screen.x) / 8;
				left.second = (mouse.y + screen.y) / 8;
			}

			if (Broodwar->getMouseState(MouseButton::M_RIGHT)) {
				Position mouse = Broodwar->getMousePosition();
				Position screen = Broodwar->getScreenPosition();
				right.first = (mouse.x + screen.x) / 8;
				right.second = (mouse.y + screen.y) / 8;

				path = bc.pathFinder.findPath(left, right);
			}

			if (path && path->hasPath) {
				PairUint last = path->path.front();
				for (PairUint pos : path->path) {
					Broodwar->drawLineMap(Position(last.first * 8 + 4, last.second * 8 + 4), Position(pos.first * 8 + 4, pos.second * 8 + 4), Colors::Orange);
					last = pos;
				}
			}

			Broodwar->drawBoxMap(Position(left.first * 8 + 3, left.second * 8 + 3), Position(left.first * 8 + 5, left.second * 8 + 5), Colors::Purple, true);
			Broodwar->drawBoxMap(Position(right.first * 8 + 3, right.second * 8 + 3), Position(right.first * 8 + 5, right.second * 8 + 5), Colors::White, true);

			// Draw nodes in open list
			int numberOfShapes = 0;
			if (path) {
				for (PairUint pos : path->visitedNodes) {
					Broodwar->drawBoxMap(Position(pos.first * 8 + 1, pos.second * 8 + 1), Position(pos.first * 8 + 7, pos.second * 8 + 7), Colors::Orange);
					if (numberOfShapes++ > 19000) break;
				}
			}
		});

		addCommand("mousetile", "Draws the tile number at mouse position", false, false, [&bc = bc]() {
			Broodwar->drawTextMouse(0, -10, "%i, %i", (Broodwar->getMousePosition().x + Broodwar->getScreenPosition().x) / 32, (Broodwar->getMousePosition().y + +Broodwar->getScreenPosition().y) / 32);
		});
	}


	template <class UnaryFunction>
	void Debug::addCommand(std::string command, std::string description, bool active, bool executeOnce, UnaryFunction action) {
		commands.emplace_back(command, description, active, executeOnce, action);
	}

	void Debug::onFrame() {
		for (auto& f : cellFunctions) {
			for (unsigned int x = 0; x < bc.map.tileWidth; x++)
				for (unsigned int y = 0; y < bc.map.tileHeight; y++)
					f(x, y, bc.map.tiles[x][y]);
		}

		for (auto& f : miniCellFunctions) {
			for (unsigned int x = 0; x < bc.map.miniTileWidth; x++)
				for (unsigned int y = 0; y < bc.map.miniTileHeight; y++)
					f(x, y, bc.map.miniTiles[x][y]);
		}


		for (auto& command : commands) {
			if (command.isActive) {
				command.action();

				if (command.executeOnce)
					command.isActive = false;
			}
		}
	}

	bool Debug::command(std::string text) {
		for (auto& command : commands) {
			if (command.command == text) {
				command.isActive = !command.isActive;

				if (!command.executeOnce)
					Broodwar->sendText("%s (%s) turned %s", command.command.c_str(), command.description.c_str(), command.isActive ? "ON" : "OFF");

				return true;
			}
		}
		return false;
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

	template<class UnaryFunction>
	void Debug::runOnEveryCell(UnaryFunction action) {
		cellFunctions.push_back(action);
	}
	template<class UnaryFunction>
	void Debug::runOnEveryMiniCell(UnaryFunction action) {
		miniCellFunctions.push_back(action);
	}

	// #######################################################
	// ############# Debug Performance Display ###############
	// #######################################################

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

	void DebugPerformanceDisplay::updateAndDraw(BlackCrow& bc, int xStart, int yStart) {
		//int logicalFrameSpeed = BW::BWDATA::GameSpeedModifiers.gameSpeedModifiers[0]; // TODO when getLocalSpeed gets implemented
		double logicalFrameSpeed = bc.getAverageFrameTime();

		// Don't show if speed is 0
		if (logicalFrameSpeed <= 0 || logicalFrameSpeed >= 500)
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