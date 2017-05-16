#pragma once
#include <BWAPI.h>
#include "Planned.h"

namespace BlackCrow {

	class BlackCrow;

	class DebugPerformanceDisplay {
	public:
		void elapsedTime(double millis);
		void updateAndDraw(int x, int y);
		void setBackgroundColor(BWAPI::Color color);
		void setShowPercentage(double percentage);
		void setShowSpikes(bool show);

	private:
		std::list<double> frameTimeHistory;
		double highestFrameTime = 0;
		double highestFrameTimeAgo = 0;
		BWAPI::Color backgroundColor = BWAPI::Colors::Brown;
		double showPercentage = 0;
		bool showSpikes = true;
	};

	// Draws debug information
	class Debug {

	public:
		Debug(BlackCrow &parent);

		void onStart();
		void drawOnFrame();

		bool command(std::string text);
		bool fastDrawBuildable;
		DebugPerformanceDisplay displayBot;
		DebugPerformanceDisplay displayBroodwar;

	private:
		BlackCrow &bc;
		std::unordered_map<Planned::Status, std::string> plannedStatusStrings;

		bool showBaseInfo;
		bool showBuildable;
		bool showManagerInfos;
		bool showPlacementInfos;
		bool showBwem;
		bool showSquadInfo;
		bool showEnemyUnits;
		bool showLifeBars;
		std::string getOnOffString(bool value);

		void drawBaseInformation();
		void drawBuildable();
		void drawManagerInfo();
		void drawPlacementInfo();
		void drawBwem();
		void drawSquadInfo();
		void drawEnemyUnits();
		void drawLifeBars();

		void drawFrameTimeDisplay();

		void drawTestPath();
	};
}