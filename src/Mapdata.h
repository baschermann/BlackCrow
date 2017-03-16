#pragma once
#include <BWAPI.h>
#include <vector>
#include <BWEM/bwem.h> 

// Draws debug information
class Mapdata {

public:
	struct Cell {
		bool buildable;
		bool resourceBuildable;
	};

	struct MiniCell {
		bool walkable;
		bool chokepoint;
	};

	int tileWidth; // Tiles TILE_SIZE or 32 pixels per tile
	int tileHeight;
	int width;
	int height;
	std::vector<std::vector<Cell>> mapTiles;

	Mapdata();
	~Mapdata();
	void onStart();

private:
	
};

extern Mapdata* mapdata;