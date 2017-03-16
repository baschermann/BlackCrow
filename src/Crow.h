#pragma once
#include <BWAPI.h>
#include "MacroManager.h"
#include "StrategyManager.h"
#include "Mapdata.h"
#include "Debug.h"
#include "Enemy.h"
#include "Config.h"
#include <iostream>
#include <assert.h>
#include <chrono>
#include "BuildingManager.h"


// Remember not to use "Broodwar" in any global class constructor!

class Crow : public BWAPI::AIModule {
public:
  // Virtual functions for callbacks, leave these as they are.
  virtual void onStart();
  virtual void onEnd(bool isWinner);
  virtual void onFrame();
  virtual void onSendText(std::string text);
  virtual void onReceiveText(BWAPI::Player player, std::string text);
  virtual void onPlayerLeft(BWAPI::Player player);
  virtual void onNukeDetect(BWAPI::Position target);
  virtual void onUnitDiscover(BWAPI::Unit unit);
  virtual void onUnitEvade(BWAPI::Unit unit);
  virtual void onUnitShow(BWAPI::Unit unit);
  virtual void onUnitHide(BWAPI::Unit unit);
  virtual void onUnitCreate(BWAPI::Unit unit);
  virtual void onUnitDestroy(BWAPI::Unit unit);
  virtual void onUnitMorph(BWAPI::Unit unit);
  virtual void onUnitRenegade(BWAPI::Unit unit);
  virtual void onSaveGame(std::string gameName);
  virtual void onUnitComplete(BWAPI::Unit unit);
  // Everything below this line is safe to modify.
  
  Crow();
  ~Crow();
  void emptyMethod();
  void init();
  void test();

private:
	std::chrono::high_resolution_clock::time_point lastFrame;
};

extern Crow* crow;
