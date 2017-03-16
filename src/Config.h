#pragma once
#include <BWAPI.h>

// Config!
class Config {

public:
	Config();

	int localSpeed;
	int maxDrones;
	float saturationLevel; 
	float barSize;
};

extern Config* config;