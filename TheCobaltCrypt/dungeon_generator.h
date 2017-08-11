#pragma once
#include <random>
#include <vector>
#include <iostream>
#include "randomOperations.h"


struct Rect
{
	int x, y;
	int width, height;
};

class Dungeon
{
public:
	enum Tile
	{
		Unused = '0',
		Floor = '1',
		Corridor = '2',
		Wall = '3',
		Decoration = '4',
		HealthPickup = '5',
		Spawn = '6',
		Exit = '7'
	};

	enum Direction
	{
		North,
		South,
		West,
		East,
		DirectionCount
	};

public:
	Dungeon(int width, int height);

	void generate(int maxFeatures);

	void print();
	
	char getTile(int x, int y) const;
	 
	int countWallTiles();

	int countFloorTiles();

	int countDecoTiles();

	int countPickupTiles();

	void clearDungeon(int width, int heigth);

private:
	void setTile(int x, int y, char tile);

	bool createFeature();

	bool createFeature(int x, int y, Direction dir);

	bool makeRoom(int x, int y, Direction dir, bool firstRoom = false);

	bool makeCorridor(int x, int y, Direction dir);

	bool placeRect(const Rect& rect, char tile);

	bool placeObject(char tile);

private:
	int _width, _height;

	RandomOperations randOps;

	std::vector<char> _tiles;

	std::vector<Rect> _rooms; // Räume

	std::vector<Rect> _exits; // 4 Seiten der Räume / Korridore
};

