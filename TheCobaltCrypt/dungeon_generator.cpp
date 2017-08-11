#include "dungeon_generator.h"

Dungeon::Dungeon(int width, int height)
	: _width(width)
	, _height(height)
	, _tiles(width * height, Unused)
	, _rooms()
	, _exits()
{
}

void Dungeon::generate(int maxFeatures)
{
	
	if (!makeRoom(_width / 2, _height / 2, static_cast<Direction>(randOps.randomInt(4), true)))  // ersten Raum im Zentrum anlegen
	{
		std::cout << "Kann ersten Raum nicht anlegen" << std::endl;
		return;
	}

	
	for (int i = 1; i < maxFeatures; ++i)	//da schon ein Raum angelegt wurde von der 1 bis zur maximalen Anzahl an Räumen...
	{
		if (!createFeature())			//Räume anlegen
		{
			std::cout << "Kann keine weiteren Räume platzieren (" << i << " Räume platziert)." << std::endl;
			break;
		}
	}

	if (!placeObject(Spawn)) //Spawn platzieren
	{
		std::cout << "Kann Spawnpunkt nicht platzieren." << std::endl;
		return;
	}

	if (!placeObject(Exit))	//Ausgang platzieren
	{
		std::cout << "Kann Ausgang nicht platzieren." << std::endl;
		return;
	}

	for (int i = 0; i < maxFeatures/2; i++)
	{
		if (randOps.randomInt(8) == 1) {
			placeObject(HealthPickup);
		}
		else
		{
			placeObject(Decoration);
		}
		
	}

	for (char& tile : _tiles)
	{
		if (tile == Unused)
			tile = ' ';			//Unbenutzte Kacheln "leermachen"
		else if (tile == Floor || tile == Corridor)
			tile = '1';			//Korridore und Räume bekommen die gleiche Textur
	}
}

//void Dungeon::print()  //Ausgabe in der Konsole (Debug) 
//{
//	for (int y = 0; y < _height; ++y)
//	{
//		for (int x = 0; x < _width; ++x)
//			std::cout << getTile(x, y);
//
//		std::cout << std::endl;
//	}
//}


char Dungeon::getTile(int x, int y) const  //entsprechende Kachel zurückgeben
{
	if (x < 0 || y < 0 || x >= _width || y >= _height)
		return Unused;

	return _tiles[x + y * _width];
}

int Dungeon::countWallTiles()		//Wandstücke zählen
{
	int walls = 0;

	for (int y = 0; y < _height; ++y)
	{
		for (int x = 0; x < _width; ++x) 
		{
			if (getTile(x, y) == '3') 
			{
				walls++;
			}

		}
	}
	return walls;
}

int Dungeon::countFloorTiles()		//Bodenstücke zählen
{
	int floors = 0;

	for (int y = 0; y < _height; ++y)
	{
		for (int x = 0; x < _width; ++x)
		{
			if (getTile(x, y) == '1' )
			{
				floors++;
			}
		}
	}
	return floors;
}

int Dungeon::countDecoTiles()		//Dekostücke zählen
{
	int deco = 0;

	for (int y = 0; y < _height; ++y)
	{
		for (int x = 0; x < _width; ++x)
		{
			if (getTile(x, y) == '1')
			{
				deco++;
			}
		}
	}
	return deco;
}

int Dungeon::countPickupTiles()
{
	int pickup = 0;

	for (int y = 0; y < _height; ++y)
	{
		for (int x = 0; x < _width; ++x)
		{
			if (getTile(x, y) == '1')
			{
				pickup++;
			}
		}
	}
	return pickup;
}

void Dungeon::clearDungeon(int width, int height) {		//Dungeon löschen
	_tiles.clear();
	_tiles.resize(width*height);
	
	_rooms.clear();
	
	_exits.clear();

	for (int i = 0; i < _tiles.size(); i++) {
		_tiles[i] = Unused;
	}
}

void Dungeon::setTile(int x, int y, char tile)		//Feld auf einen bestimmten Typ setzen
{
	_tiles[x + y * _width] = tile;
}

bool Dungeon::createFeature()
{
	for (int i = 0; i < 1000; ++i)
	{
		if (_exits.empty())
			break;

		
		int r = randOps.randomInt(_exits.size()); //Zufällige Seite eines zufälligen Raumes / Korridors wählen
		int x = randOps.randomInt(_exits[r].x, _exits[r].x + _exits[r].width - 1);
		int y = randOps.randomInt(_exits[r].y, _exits[r].y + _exits[r].height - 1);

		// Norden, Süden, Westen, Osten
		for (int j = 0; j < DirectionCount; ++j)
		{
			if (createFeature(x, y, static_cast<Direction>(j)))
			{
				_exits.erase(_exits.begin() + r);
				return true;
			}
		}
	}

	return false;
}

bool Dungeon::createFeature(int x, int y, Direction dir)
{
	static const int roomChance = 60; // corridorChance = 100 - roomChance

	int dx = 0;
	int dy = 0;

	if (dir == North)
		dy = 1;
	else if (dir == South)
		dy = -1;
	else if (dir == West)
		dx = 1;
	else if (dir == East)
		dx = -1;

	if (getTile(x + dx, y + dy) != Floor && getTile(x + dx, y + dy) != Corridor)
		return false;

	if (randOps.randomInt(100) < roomChance)
	{
		if (makeRoom(x, y, dir))
		{
			setTile(x, y, Floor); //Türen (im Moment keine Textur -> daher Fußboden)

			return true;
		}
	}

	else
	{
		if (makeCorridor(x, y, dir))
		{
			if (getTile(x + dx, y + dy) == Floor)
				setTile(x, y, Floor); //Korridor-Türen (im Moment keine Textur -> daher Fußboden)
			else // Keine Türen zwischen Korridoren platzieren
				setTile(x, y, Corridor);

			return true;
		}
	}

	return false;
}

bool Dungeon::makeRoom(int x, int y, Direction dir, bool firstRoom ) //Raum generieren
{
	static const int minRoomSize = 3;	//minimale und maximale Raumgröße
	static const int maxRoomSize = 8;

	Rect room;
	room.width = randOps.randomInt(minRoomSize, maxRoomSize);
	room.height = randOps.randomInt(minRoomSize, maxRoomSize);

	if (dir == North)
	{
		room.x = x - room.width / 2;
		room.y = y - room.height;
	}

	else if (dir == South)
	{
		room.x = x - room.width / 2;
		room.y = y + 1;
	}

	else if (dir == West)
	{
		room.x = x - room.width;
		room.y = y - room.height / 2;
	}

	else if (dir == East)
	{
		room.x = x + 1;
		room.y = y - room.height / 2;
	}

	if (placeRect(room, Floor))
	{
		_rooms.emplace_back(room);

		if (dir != South || firstRoom) // Nordseite
			_exits.emplace_back(Rect{ room.x, room.y - 1, room.width, 1 });
		if (dir != North || firstRoom) // Südseite
			_exits.emplace_back(Rect{ room.x, room.y + room.height, room.width, 1 });
		if (dir != East || firstRoom) // Westseite
			_exits.emplace_back(Rect{ room.x - 1, room.y, 1, room.height });
		if (dir != West || firstRoom) // Ostseite
			_exits.emplace_back(Rect{ room.x + room.width, room.y, 1, room.height });

		return true;
	}

	return false;
}
bool Dungeon::makeCorridor(int x, int y, Direction dir)
{
	static const int minCorridorLength = 3;
	static const int maxCorridorLength = 6;

	Rect corridor;
	corridor.x = x;
	corridor.y = y;

	if (randOps.randomBool()) // horizontaler Korridor
	{
		corridor.width = randOps.randomInt(minCorridorLength, maxCorridorLength);
		corridor.height = 1;

		if (dir == North)
		{
			corridor.y = y - 1;

			if (randOps.randomBool()) // Westen
				corridor.x = x - corridor.width + 1;
		}

		else if (dir == South)
		{
			corridor.y = y + 1;

			if (randOps.randomBool()) // Westen
				corridor.x = x - corridor.width + 1;
		}

		else if (dir == West)
			corridor.x = x - corridor.width;

		else if (dir == East)
			corridor.x = x + 1;
	}

	else // vertikaler Korridor
	{
		corridor.width = 1;
		corridor.height = randOps.randomInt(minCorridorLength, maxCorridorLength);

		if (dir == North)
			corridor.y = y - corridor.height;

		else if (dir == South)
			corridor.y = y + 1;

		else if (dir == West)
		{
			corridor.x = x - 1;

			if (randOps.randomBool()) // norden
				corridor.y = y - corridor.height + 1;
		}

		else if (dir == East)
		{
			corridor.x = x + 1;

			if (randOps.randomBool()) // Norden
				corridor.y = y - corridor.height + 1;
		}
	}

	if (placeRect(corridor, Corridor))
	{
		if (dir != South && corridor.width != 1) // nördliche Seite
			_exits.emplace_back(Rect{ corridor.x, corridor.y - 1, corridor.width, 1 });
		if (dir != North && corridor.width != 1) // südliche Seite
			_exits.emplace_back(Rect{ corridor.x, corridor.y + corridor.height, corridor.width, 1 });
		if (dir != East && corridor.height != 1) // westliche Seite
			_exits.emplace_back(Rect{ corridor.x - 1, corridor.y, 1, corridor.height });
		if (dir != West && corridor.height != 1) // östliche Seite
			_exits.emplace_back(Rect{ corridor.x + corridor.width, corridor.y, 1, corridor.height });

		return true;
	}

	return false;
}

bool Dungeon::placeRect(const Rect& rect, char tile)
{
	if (rect.x < 1 || rect.y < 1 || rect.x + rect.width > _width - 1 || rect.y + rect.height > _height - 1)
		return false;

	for (int y = rect.y; y < rect.y + rect.height; ++y)
		for (int x = rect.x; x < rect.x + rect.width; ++x)
		{
			if (getTile(x, y) != Unused)
				return false; // Bereich schon genutzt
		}

	for (int y = rect.y - 1; y < rect.y + rect.height + 1; ++y)
		for (int x = rect.x - 1; x < rect.x + rect.width + 1; ++x)
		{
			if (x == rect.x - 1 || y == rect.y - 1 || x == rect.x + rect.width || y == rect.y + rect.height)
				setTile(x, y, Wall);
			else
				setTile(x, y, tile);
		}

	return true;
}

bool Dungeon::placeObject(char tile)
{
	if (_rooms.empty())
		return false;

	int r = randOps.randomInt(_rooms.size()); // zufälligen Raum wählen
	int x = randOps.randomInt(_rooms[r].x + 1, _rooms[r].x + _rooms[r].width - 2);
	int y = randOps.randomInt(_rooms[r].y + 1, _rooms[r].y + _rooms[r].height - 2);
	int i = 0;

	if (getTile(x, y) == Floor)
	{
		setTile(x, y, tile);
		
		// nur ein Objekt in einem Raum platzieren (optional)
		//_rooms.erase(_rooms.begin() + r);

		return true;
	}

	return false;
}

