#pragma once
#include <BWAPI.h>
//#include <BW/Offsets.h>

namespace BlackCrow {

	class BlackCrow;

	// Draws debug information
	class Debug {

	public:
		Debug(BlackCrow &parent);
		~Debug();

		void onStart();
		void drawOnFrame();
		bool command(std::string text);
		double elapsedMs;
		std::list<float>* frameTimeHistory;
		double highestFrameTime;
		double highestFrameTimeAgo;

	private:
		BlackCrow &bc;

		bool showBaseInfo;
		bool showBuildable;
		bool showResourceBuildable;
		bool showManagerInfos;
		bool showPlacementInfos;
		bool showBwem;
		bool showSquadInfo;
		bool showEnemyUnits;
		bool showLifeBars;
		std::string getOnOffString(bool value);

		void drawBaseInformation();
		void drawBuildable();
		void drawResourceBuildable();
		void drawManagerInfo();
		void drawPlacementInfo();
		void drawBwem();
		void drawSquadInfo();
		void drawEnemyUnits();
		void drawLifeBars();

		void drawFrameTimeDisplay();
	};
}