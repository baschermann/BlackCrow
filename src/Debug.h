#pragma once
#include <BWAPI.h>
#include "MacroManager.h"
#include "StrategyManager.h"
#include "Mapdata.h"
#include "Enemy.h"
#include "Util.h"
#include "Config.h"
#include <BWEM/bwem.h>
#include <chrono>
//#include <BW/Offsets.h>


// Draws debug information
class Debug {

public:
	Debug();
	void onStart();
	void drawOnFrame();
	bool command(std::string text);
	double elapsedMs;
	std::list<float>* frameTimeHistory;
	double highestFrameTime;
	double highestFrameTimeAgo;

private:
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

extern Debug* debug;

