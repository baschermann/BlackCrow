#pragma once
#include <BWAPI.h>
#include "Planned.h"
//#include <BW/Offsets.h>


namespace BlackCrow {

	class BlackCrow;

	// Draws debug information
	class Debug {

	public:
		Debug(BlackCrow &parent);

		void onStart();
		void drawOnFrame();
		bool command(std::string text);
		double elapsedMs;
		std::list<double> frameTimeHistory;
		double highestFrameTime;
		double highestFrameTimeAgo;

		bool fastDrawBuildable;

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
	};
}