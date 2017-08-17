#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>
#include <iostream>
#include <stdio.h>      
#include <stdlib.h>
#include <random>
#include "dungeon_generator.h"
#include "player.h"
#include "enemy.h"


const int MAIN_MENU = 0;
const int GAME = 1;
const int GAME_OVER = 3;
const int BLUE = 3;
const int YELLOW = 4;
const int FRAMERATE = 60;
const sf::Vector2f TEXTURE_SIZE = { 16.0f, 16.0f };							//Größe der verwendeten Texturen

Dungeon d(79,45);		// Dungeon d(Breite des Fensters / Texturgröße, Höhe des Fensters / Texturgröße) -> 1280 / 16 && 736 / 16

std::vector<std::pair<sf::RectangleShape, bool> > groundTiles;	//Vektor zum Organisieren der Bodenkacheln, sowie deren Sichtbarkeit
std::vector<std::pair<sf::RectangleShape, bool> > wallTiles;    //Vektor zum Organisieren der Wandkacheln, sowie deren Sichtbarkeit
std::vector<std::pair<sf::RectangleShape, bool> > decorationTiles;
std::vector<std::pair<sf::RectangleShape, bool> > healthTiles;
std::vector<Enemy> enemies;

int rooms;
int blueDur = 15;
int misc = BLUE;
int attackTimer = 0;

int map[1280][736];												//Array um festzustellen, wo welche Bodenkachel liegt
int mapWalls[1280][736];										//Array um festzustellen, wo welche Wandkachel liegt
int mapDecorations[1280][736];
int mapPickups[1280][736];

sf::Texture main_menu[2];
sf::Texture game_over[2];
sf::Sprite mmText[2];
sf::Sprite goText[2];

sf::Sprite mmBackground;
sf::Sprite goBackground;
sf::Sprite healthFront;
sf::Sprite healthBack;
sf::Sprite healthContent;

sf::Texture wallTextures[6];									//Wandtextur
sf::Texture groundTextures[6];									//Bodentextur
sf::Texture decorations[34];
sf::Texture monsters[6];

sf::Texture t_healthBack;
sf::Texture t_healthFront;
sf::Texture t_healthContent;
sf::Texture t_healthPickup;

sf::RectangleShape spawnpoint(sf::Vector2f(16.0f, 16.0f));		//Rechteck, welches den Spawnpunkt markiert. Immer sichtbar

std::pair<int, int> spawnChoords;				//Pair, welches die Startkoordinaten enthält
std::pair<sf::RectangleShape, bool> trapdoor;	//Pair, welches den rechteckigen Ausgang, sowie dessen Sichtbarkeit regelt

std::random_device radev;	
std::mt19937 eng(radev());
std::uniform_int_distribution<> distr(80, 200);
std::uniform_int_distribution<> rnd(0, 5);
std::uniform_int_distribution<> rndDeco(0, 33);

sf::Music menuMusic;
sf::Music gameOverMusic;


void resetTextures() {
	for (int y = 0; y < 736; y++) {
		for (int x = 0; x < 1280; x++)
		{
			map[x][y] = 0;						//die beiden Arrays "leeren"
			mapWalls[x][y] = 0;
			mapDecorations[x][y] = 0;
			mapPickups[x][y] = 0;
		}
	}

	for (int i = 0; i < groundTiles.size(); i++)
	{
		groundTiles[i].first.setTexture(&groundTextures[rnd(eng)]);		//die Vektoren zurücksetzen
		groundTiles[i].second = false;
		groundTiles[i].first.setSize(TEXTURE_SIZE);
	}

	for (int k = 0; k < decorationTiles.size(); k++)
	{
		decorationTiles[k].first.setTexture(&decorations[rndDeco(eng)]);
		decorationTiles[k].second = false;
		groundTiles[k].first.setSize(TEXTURE_SIZE);
	}

	for (int j = 0; j < wallTiles.size(); j++)
	{
		wallTiles[j].first.setTexture(&wallTextures[rnd(eng)]);
		wallTiles[j].second = false;
		wallTiles[j].first.setSize(TEXTURE_SIZE);
	}

	healthContent.setScale(1.5f, 1.5f);
}

void prepareTextures() {
	groundTiles.resize(d.countFloorTiles());	//Boden- und Wandkachelvektor auf die der Anzahl an Kacheln entsprechende Größe bringen
	wallTiles.resize(d.countWallTiles());
	decorationTiles.resize(d.countDecoTiles());
	healthTiles.resize(d.countPickupTiles());
	enemies.reserve(div(rooms, 8).quot);

	
	t_healthBack.loadFromFile("lifebar_back.png");
	t_healthFront.loadFromFile("lifebar_front.png");
	t_healthContent.loadFromFile("lifebar_content.png");
	t_healthPickup.loadFromFile("heal.png");

	healthBack.setTexture(t_healthBack);
	healthFront.setTexture(t_healthFront);
	healthContent.setTexture(t_healthContent);

	healthBack.setPosition(0.0f, 0.0f);
	healthFront.setPosition(0.0f, 0.0f);
	healthContent.setPosition(21.0f, 16.5f);

	healthBack.setScale(1.5f, 1.5f);
	healthFront.setScale(1.5f, 1.5f);
	healthContent.setScale(1.5f, 1.5f);

	for (int i = 0; i < groundTiles.size(); i++)
	{
		groundTiles[i].first.setTexture(&groundTextures[rnd(eng)]);	//Alle Bodenkacheln mit Textur versehen
		groundTiles[i].second = false;						//...zum Spielstart ausblenden
		groundTiles[i].first.setSize(TEXTURE_SIZE);			//und auf die richtige Größe bringen
	}

	for (int j = 0; j < wallTiles.size(); j++)
	{
		wallTiles[j].first.setTexture(&wallTextures[rnd(eng)]);		//s.o. nur mit den Wandkacheln
		wallTiles[j].second = false;
		wallTiles[j].first.setSize(TEXTURE_SIZE);
	}

	for (int k = 0; k < decorationTiles.size(); k++)
	{
		decorationTiles[k].first.setTexture(&decorations[rndDeco(eng)]);
		decorationTiles[k].second = false;
		decorationTiles[k].first.setSize(TEXTURE_SIZE);
	}

	for (int l = 0; l < div(rooms,8).quot; l++)	{
		Enemy x(&monsters[rnd(eng)], sf::Vector2u(5, 4), 0.2f, 100.0f, 110, 10);
		enemies.push_back(x);
		enemies[l].isVisible = false;
	}

	for (int m = 0; m < healthTiles.size(); m++)
	{
		healthTiles[m].first.setTexture(&t_healthPickup);
		healthTiles[m].second = false;
		healthTiles[m].first.setSize(TEXTURE_SIZE);
	}
}

int processMainMenu(sf::RenderWindow& window) {
	
	if (misc == BLUE) 
	{
		window.draw(mmText[0]);

		if (blueDur <= 0)
			misc = YELLOW;

		blueDur--;
	}
	else
	{
		window.draw(mmText[1]);
		
		if (blueDur >= 15)
			misc = BLUE;

		blueDur++;
	}
		
		window.display();

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Return)) {
			misc = BLUE;
			blueDur = 15;
			return GAME;
		}

		
		return MAIN_MENU;

}


int processGameOver(sf::RenderWindow& window) {

	if (misc == BLUE)
	{
		window.draw(goText[0]);

		if (blueDur <= 0)
			misc = YELLOW;

		blueDur--;
	}
	else
	{
		window.draw(goText[1]);

		if (blueDur >= 15)
			misc = BLUE;

		blueDur++;
	}

	window.display();

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
		misc = BLUE;
		blueDur = 15;
		gameOverMusic.stop();
		menuMusic.play();
		return MAIN_MENU;
	}


	return GAME_OVER;

}

//_________________Erzeugen___des___Dungeons_________________//
void generateDungeon() {
	
	rooms = distr(eng);  //Erzeugen einer Zufallszahl zwischen 80 und 200
	d.generate(rooms);		//Erzeugen des Dungeons mit der generierten Zahl
	std::cout << "Abgeschlossen. \n Dungeon sollte jetzt " << rooms << " Raeume und Korridore haben." << std::endl;
	//d.print(); //Ausgabe in Konsole (nur für Debugging mit kleineren Werten -> sonst unlesbar)

}

bool isWall(int offsetX, int offsetY, Player player) {

	if (d.getTile((player.getPosition().x + offsetX) / 16, (player.getPosition().y + offsetY) / 16) == '1'
		|| d.getTile((player.getPosition().x + offsetX) / 16, (player.getPosition().y + offsetY) / 16) == '4'
		|| d.getTile((player.getPosition().x + offsetX) / 16, (player.getPosition().y + offsetY) / 16) == '5'
		|| d.getTile((player.getPosition().x + offsetX) / 16, (player.getPosition().y + offsetY) / 16) == '6'
		|| d.getTile((player.getPosition().x + offsetX) / 16, (player.getPosition().y + offsetY) / 16) == '7') //angrenzende Kachel überprüfen (da man nicht durch Wände gehen soll)
	{
		return false;
	}
	return true;

}

//_____Übertragen_des_Dungeons_in_ein_für_SFML_verarbeitbares_Format_____//
void showDungeon() {
	int ground = 0;
	int wall = 0;
	int deco = 0;
	int health = 0;

	for (int y = 0; y < 45; y++) {
		for (int x = 0; x < 79; x++)
		{
			switch (d.getTile(x,y))		//Auslesen des Kacheltyps and der bestimmten Stelle
			{
			case '1':					//Bodenkachel
				groundTiles[ground].first.setPosition(x * 16, y * 16);	//Entsprechende Kachel aus dem Vektor an die bestimmte Stelle setzen
				for (int i = y*16; i < (y+1)*16; i++)
				{
					for (int j = x*16; j < (x+1)*16; j++)
					{
						map[j][i] = ground;								//Da ich 16px * 16px Texturen verwende, müssen in dem Array 256 Plätze für eine Kachel reserviert werden
					}
				}
				ground++;	//Kachelzähler
				break;
			case '3':					//Wandkachel
				wallTiles[wall].first.setPosition(x * 16, y * 16);		//s.o. nur für Wandkacheln
				for (int k = y * 16; k < (y + 1) * 16; k++)
				{
					for (int l = x * 16; l < (x + 1) * 16; l++)
					{
						mapWalls[l][k] = wall;							//auch hier müssen 256 Plätze reserviert werden
					}
				}
				wall++;
				break;
			case '4':
				decorationTiles[deco].first.setPosition(x * 16, y * 16);		
				for (int m = y * 16; m < (y + 1) * 16; m++)
				{
					for (int n = x * 16; n < (x + 1) * 16; n++)
					{
						mapDecorations[n][m] = deco;							//auch hier müssen 256 Plätze reserviert werden
					}
				}
				deco++;
				break;
			case '5': 
				healthTiles[health].first.setPosition(x * 16, y * 16);
				for (int m = y * 16; m < (y + 1) * 16; m++)
				{
					for (int n = x * 16; n < (x + 1) * 16; n++)
					{
						mapPickups[n][m] = health;							//auch hier müssen 256 Plätze reserviert werden
					}
				}
				health++;
				break;
			case '6':					//Spawnpunkt
				spawnpoint.setPosition(x * 16, y * 16);					//Spawnpunkt an seine Position setzen
				spawnChoords.first = x * 16;							//Pair "spawnChoords" mit Werten versehen
				spawnChoords.second = y * 16;
				break;
			case '7':					//Ausgang
				trapdoor.first.setPosition(x * 16, y * 16);				//Ausgang an seine Position setzen
				trapdoor.second = false;								//Ausgang ist am Levelbeginn nicht sichtbar
				break;
			}
		}
	}

}




int main() {

	sf::RenderWindow window(sf::VideoMode(1280, 720), "The Cobalt Crypt");	//Spielfenster anlegen

	window.setFramerateLimit(FRAMERATE);
	int gameState = 0;

	std::cout << "Lade Texturen..." << std::endl;
	
	sf::Texture playerTexture;
	playerTexture.loadFromFile("warrior.png");

	Player player(&playerTexture, sf::Vector2u(10, 10), 0.1f, 100.0f,100, 20,0);

	
	sf::Texture bg;
	bg.loadFromFile("Main Menu.png");
	mmBackground.setTexture(bg);
	
	sf::Texture gameOverBg;
	gameOverBg.loadFromFile("GameOver.png");
	goBackground.setTexture(gameOverBg);

	for (int i = 0; i < 2; i++)
	{
		main_menu[i].loadFromFile("text" + std::to_string(i + 2) + ".png");
		mmText[i].setTexture(main_menu[i]);
	}

	for (int i = 0; i < 2; i++)
	{
		game_over[i].loadFromFile("GOtext" + std::to_string(i) + ".png");
		goText[i].setTexture(game_over[i]);
	}


	float deltaTime = 0.0f;
	sf::Clock clock;
										
	for (int i = 0; i < 6; i++)
	{

		groundTextures[i].loadFromFile("floor_var" + std::to_string(i + 1) + ".png");
	}


	for (int i = 0; i < 6; i++)
	{

		wallTextures[i].loadFromFile("wall_var" + std::to_string(i + 1) + ".png");
	}

	
	
	for (int i = 0; i < 34; i++)
	{
		
		decorations[i].loadFromFile("decoration" + std::to_string(i+1) + ".png");
	}

	
	for (int i = 0; i < 6; i++)
	{

		monsters[i].loadFromFile("monster" + std::to_string(i + 1) + ".png");
	}
	

	sf::Texture exitTile;													//Textur des Ausgangs
	exitTile.loadFromFile("exitTile.png");

	sf::Texture spawnTile;													//Textur des Spawnpunktes
	spawnTile.loadFromFile("spawn.png");

	trapdoor.first.setTexture(&exitTile);									//Textur des Ausganges verwenden		
	trapdoor.first.setSize(TEXTURE_SIZE);

	spawnpoint.setTexture(&spawnTile);										//Textur des Spawnpunktes verwenden


	std::cout << "Texturen geladen." << std::endl;

	sf::Music ambient;
	ambient.openFromFile("Dungeon.ogg");
	ambient.setLoop(true);

	
	menuMusic.openFromFile("Menu.ogg");
	menuMusic.setLoop(true);
	menuMusic.play();

	
	gameOverMusic.openFromFile("GameOver.ogg");
	gameOverMusic.setLoop(true);

	bool playing = true;
	bool menuPlaying = true;
	bool gameOverPlaying = false;
	bool checkedOnce = true;





//________________________SPIELBEGINN___________________________

	while (window.isOpen())			//während das Fenster geöffnet ist
	{

		deltaTime = clock.restart().asSeconds();

		window.clear();				//alles löschen

		switch (gameState)
		{
		case MAIN_MENU:
			menuPlaying = true;
			window.draw(mmBackground);
			gameState = processMainMenu(window);
			checkedOnce = true;
			break;
		case GAME_OVER:
			gameOverPlaying = true;
			window.draw(goBackground);
			gameState = processGameOver(window);
		}


		sf::Event evnt;
		while (window.pollEvent(evnt))	//Fenster für Eingaben verwendbar machen
		{
			switch (evnt.type)
			{
			case sf::Event::Closed:		//Wenn man das Fenster schließt
					window.close();		//Fenster schließen
					break;
			case sf::Event::Resized:	//Wenn man die Größe des Fensters ändert
				std::cout << "Neue Breite: " << evnt.size.width << std::endl;	//Neue Höhe und Breite ausgeben
				std::cout << "Neue Hoehe: " << evnt.size.height << std::endl;
					break;
			case sf::Event::KeyReleased:
				if (evnt.key.code == sf::Keyboard::M)
				{
					if (playing)
					{
						if (menuPlaying) 
						{
							menuMusic.stop();
						}
						else
						{
							ambient.stop();
						}
						
						playing = false;
					}
					else
					{
						if (menuPlaying)
						{
							menuMusic.play();
						}
						else
						{
							ambient.play();
						}

						playing = true;
					}
				}
				if (evnt.key.code == sf::Keyboard::Subtract)
				{
					player.takeDamage(10);
				}
			}
				
			
		}

	
		if (gameState == GAME) {

			menuPlaying = false;

			if (checkedOnce)
			{
				std::cout << "Generiere Dungeon..." << std::endl;
				d.clearDungeon(79, 45);
				generateDungeon();							//Dungeon generieren
				prepareTextures();

				player.resurrect();

				for (int y = 0; y < 736; y++) {
					for (int x = 0; x < 1280; x++)
					{
						map[x][y] = 0;						//die beiden Arrays "leeren"
						mapWalls[x][y] = 0;
					}
				}

				showDungeon();		//Dungeon für SFML verwendbar machen

				player.SetPosition(sf::Vector2f(spawnChoords.first, spawnChoords.second));	//Spieler auf den Spawnpunkt setzen

				int posX = rand() % 1280 + 1;
				int posY = rand() % 736 + 1;

				for (int l = 0; l < enemies.size(); l++)
				{
					while (map[posX][posY] == 0)
					{
						posX = rand() % 1280 + 1;
						posY = rand() % 736 + 1;
					}
					
					enemies[l].SetPosition(sf::Vector2f(posX, posY));
					posX = 0;
					posY = 0;

				}

				menuMusic.stop();
				ambient.play();

				checkedOnce = false;
			}



			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
			{
				gameState = MAIN_MENU;

				checkedOnce = true;

				menuMusic.play();
				ambient.stop();
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) { //Wenn eine Taste gedrückt wird (hier "A")
				if (!isWall(-4, 2, player)) //angrenzende Kachel überprüfen (da man nicht durch Wände gehen soll
				{
					player.movePlayer(sf::Vector2f(-100 * deltaTime, 0.0f));	//ist es keine Wand -> Spieler bewegen
				}		
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) { //s.o. (für "D")
				if (!isWall(4, 2, player))
				{
					player.movePlayer(sf::Vector2f(100 * deltaTime, 0.0f));
				}
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) { //s.o. (für "W")
				if (!isWall(0, 0, player))
				{
					player.movePlayer(sf::Vector2f(0.0f, -100 * deltaTime));
				}
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {  //s.o. (für "S")
				if (!isWall(0, 5, player))
				{
					player.movePlayer(sf::Vector2f(0.0f, 100 * deltaTime));
				}
			}


			groundTiles[map[(int)player.getPosition().x][(int)player.getPosition().y]].second = true;			//nebenbei Boden-...
			groundTiles[map[(int)player.getPosition().x + 16][(int)player.getPosition().y]].second = true;
			groundTiles[map[(int)player.getPosition().x - 16][(int)player.getPosition().y]].second = true;
			groundTiles[map[(int)player.getPosition().x][(int)player.getPosition().y + 16]].second = true;
			groundTiles[map[(int)player.getPosition().x][(int)player.getPosition().y - 16]].second = true;
			groundTiles[map[(int)player.getPosition().x + 16][(int)player.getPosition().y -16]].second = true;			//nebenbei Boden-...
			groundTiles[map[(int)player.getPosition().x + 16][(int)player.getPosition().y +16]].second = true;
			groundTiles[map[(int)player.getPosition().x - 16][(int)player.getPosition().y -16]].second = true;
			groundTiles[map[(int)player.getPosition().x - 16][(int)player.getPosition().y + 16]].second = true;


			wallTiles[mapWalls[(int)player.getPosition().x][(int)player.getPosition().y]].second = true;		//...und Wandfelder aufdecken und sichtbar machen
			wallTiles[mapWalls[(int)player.getPosition().x + 16][(int)player.getPosition().y]].second = true;
			wallTiles[mapWalls[(int)player.getPosition().x - 16][(int)player.getPosition().y]].second = true;
			wallTiles[mapWalls[(int)player.getPosition().x][(int)player.getPosition().y + 16]].second = true;
			wallTiles[mapWalls[(int)player.getPosition().x][(int)player.getPosition().y - 16]].second = true;
			wallTiles[mapWalls[(int)player.getPosition().x + 16][(int)player.getPosition().y - 16]].second = true;			
			wallTiles[mapWalls[(int)player.getPosition().x + 16][(int)player.getPosition().y + 16]].second = true;
			wallTiles[mapWalls[(int)player.getPosition().x - 16][(int)player.getPosition().y - 16]].second = true;
			wallTiles[mapWalls[(int)player.getPosition().x - 16][(int)player.getPosition().y + 16]].second = true;


			decorationTiles[mapDecorations[(int)player.getPosition().x][(int)player.getPosition().y]].second = true;
			decorationTiles[mapDecorations[(int)player.getPosition().x + 16][(int)player.getPosition().y]].second = true;
			decorationTiles[mapDecorations[(int)player.getPosition().x - 16][(int)player.getPosition().y]].second = true;
			decorationTiles[mapDecorations[(int)player.getPosition().x][(int)player.getPosition().y + 16]].second = true;
			decorationTiles[mapDecorations[(int)player.getPosition().x][(int)player.getPosition().y - 16]].second = true;
			
			healthTiles[mapPickups[(int)player.getPosition().x][(int)player.getPosition().y]].second = true;
			healthTiles[mapPickups[(int)player.getPosition().x + 16][(int)player.getPosition().y]].second = true;
			healthTiles[mapPickups[(int)player.getPosition().x - 16][(int)player.getPosition().y]].second = true;
			healthTiles[mapPickups[(int)player.getPosition().x][(int)player.getPosition().y + 16]].second = true;
			healthTiles[mapPickups[(int)player.getPosition().x][(int)player.getPosition().y - 16]].second = true;

			/*Extra if-Verwzeigung, da der Ausgang in keinem Vector gespeichert ist und extra sichtbar gemacht werden muss*/
			if (d.getTile((player.getPosition().x + 16) / 16, (player.getPosition().y) / 16) == '7'
				|| d.getTile((player.getPosition().x - 16) / 16, (player.getPosition().y) / 16) == '7'
				|| d.getTile((player.getPosition().x) / 16, (player.getPosition().y + 16) / 16) == '7'
				|| d.getTile((player.getPosition().x) / 16, (player.getPosition().y - 16) / 16) == '7'
				|| d.getTile((player.getPosition().x - 16) / 16, (player.getPosition().y - 16) / 16) == '7'
				|| d.getTile((player.getPosition().x + 16) / 16, (player.getPosition().y - 16) / 16) == '7'
				|| d.getTile((player.getPosition().x - 16) / 16, (player.getPosition().y + 16) / 16) == '7'
				|| d.getTile((player.getPosition().x + 16 ) / 16, (player.getPosition().y + 16) / 16) == '7')
			{
				trapdoor.second = true;
			}
				
			if (d.getTile(player.getPosition().x / 16, (player.getPosition().y) / 16) == '5' 
				&& healthTiles[mapPickups[(int)player.getPosition().x][(int)player.getPosition().y]].first.getTexture() != &groundTextures[4]) {
				player.pickup();
				healthTiles[mapPickups[(int)player.getPosition().x][(int)player.getPosition().y]].first.setTexture(&groundTextures[4]);
			}


			sf::Vector2f movement(0.0f, 0.0f);

			for (int i = 0; i < enemies.size(); i++) {
				enemies[i].movement = movement;

				if (enemies[i].direction == 1)
				{
					if (d.getTile(enemies[i].getPosition().x / 16, (enemies[i].getPosition().y / 16) == '1'
						|| d.getTile(enemies[i].getPosition().x / 16, enemies[i].getPosition().y / 16) == '6'
						|| d.getTile(enemies[i].getPosition().x / 16, enemies[i].getPosition().y / 16) == '7'
						|| d.getTile(enemies[i].getPosition().x / 16, enemies[i].getPosition().y / 16)) == '4')
					{
						enemies[i].movement.y -= 100 * deltaTime;
					}
				}
				else if (enemies[i].direction == 2)
				{
					if (d.getTile(enemies[i].getPosition().x / 16, (enemies[i].getPosition().y + 5) / 16) == '1'
						|| d.getTile(enemies[i].getPosition().x / 16, (enemies[i].getPosition().y + 5) / 16) == '6'
						|| d.getTile(enemies[i].getPosition().x / 16, (enemies[i].getPosition().y + 5) / 16) == '7'
						|| d.getTile(enemies[i].getPosition().x / 16, (enemies[i].getPosition().y + 5) / 16) == '4')
					{
						enemies[i].movement.y += 100 * deltaTime;
					}
				}
				else if (enemies[i].direction == 3)
				{
					if (d.getTile((enemies[i].getPosition().x - 4) / 16, (enemies[i].getPosition().y + 2) / 16) == '1'
						|| d.getTile((enemies[i].getPosition().x - 4) / 16, (enemies[i].getPosition().y + 2) / 16) == '6'
						|| d.getTile((enemies[i].getPosition().x - 4) / 16, (enemies[i].getPosition().y + 2) / 16) == '7'
						|| d.getTile((enemies[i].getPosition().x - 4) / 16, (enemies[i].getPosition().y + 2) / 16) == '4')
					{
						enemies[i].movement.x -= 100 * deltaTime;
					}
				}
				else if (enemies[i].direction == 4)
				{
					if (d.getTile((enemies[i].getPosition().x + 4) / 16, (enemies[i].getPosition().y + 2) / 16) == '1'
						|| d.getTile((enemies[i].getPosition().x + 4) / 16, (enemies[i].getPosition().y + 2) / 16) == '6'
						|| d.getTile((enemies[i].getPosition().x + 4) / 16, (enemies[i].getPosition().y + 2) / 16) == '7'
						|| d.getTile((enemies[i].getPosition().x + 4) / 16, (enemies[i].getPosition().y + 2) / 16) == '4')
					{
						enemies[i].movement.x += 100 * deltaTime;
					}
				}
				else
				{
					//Nicht bewegen
				}

				if (groundTiles[map[(int)enemies[i].getPosition().x][(int)enemies[i].getPosition().y]].second && !enemies[i].dead)
					enemies[i].isVisible = true;
				else
				{
					enemies[i].isVisible = false;
				}

				if (map[(int)enemies[i].getPosition().x + 16][(int)enemies[i].getPosition().y] == map[(int)player.getPosition().x][(int)player.getPosition().y]
					&& map[(int)player.getPosition().x][(int)player.getPosition().y] != 0
					|| map[(int)enemies[i].getPosition().x - 16][(int)enemies[i].getPosition().y] == map[(int)player.getPosition().x][(int)player.getPosition().y]
					&& map[(int)player.getPosition().x][(int)player.getPosition().y] != 0
					|| map[(int)enemies[i].getPosition().x][(int)enemies[i].getPosition().y + 16] == map[(int)player.getPosition().x][(int)player.getPosition().y]
					&& map[(int)player.getPosition().x][(int)player.getPosition().y] != 0
					|| map[(int)enemies[i].getPosition().x][(int)enemies[i].getPosition().y - 16] == map[(int)player.getPosition().x][(int)player.getPosition().y]
					&& map[(int)player.getPosition().x][(int)player.getPosition().y] != 0)
				{
					if (!enemies[i].dead) {
						enemies[i].attacks = true;
						attackTimer++;
						if (attackTimer == 29) {
							player.takeDamage(enemies[i].dealDamage(enemies[i].strength));
							attackTimer = 0;
						}
					}
					else
					{
						enemies[i].isVisible = false;
					}
				}
			}
			if (player.attacks) {
				for (int i = 0; i < enemies.size(); i++)
				{
					if (map[(int)player.getPosition().x + 16][(int)player.getPosition().y] == map[(int)enemies[i].getPosition().x][(int)enemies[i].getPosition().y]
						|| map[(int)player.getPosition().x - 16][(int)player.getPosition().y] == map[(int)enemies[i].getPosition().x][(int)enemies[i].getPosition().y]
						|| map[(int)player.getPosition().x][(int)player.getPosition().y + 16] == map[(int)enemies[i].getPosition().x][(int)enemies[i].getPosition().y]
						|| map[(int)player.getPosition().x][(int)player.getPosition().y - 16] == map[(int)enemies[i].getPosition().x][(int)enemies[i].getPosition().y])
					{
						enemies[i].takeDamage(player.dealDamage());
					}
				}
				
			}
		
			healthContent.setScale(player.getHealth() * 1.5f / player.maxHealth, 1.5f);
			healthContent.setPosition(21 + ((player.getHealth() * 1.5f / player.maxHealth) * (-6.6667f) + 11), 16.5f);

			if (d.getTile(player.getPosition().x / 16, (player.getPosition().y) / 16) == '7') { //Erreicht der Spieler den Ausgang

				d.clearDungeon(79, 45);						//Dungeon löschen
				generateDungeon();							//neuen Dungeon erzeugen

				groundTiles.resize(d.countFloorTiles());	//die drei Vektoren auf die richtige Größe bringen
				wallTiles.resize(d.countWallTiles());
				decorationTiles.resize(d.countDecoTiles());

				resetTextures();

				showDungeon();			//Dungeon für SFML vorbereiten

				player.SetPosition(sf::Vector2f(spawnChoords.first, spawnChoords.second));	//Spieler auf Spawnpunkt setzen
			}

			

			if (player.dead) {
				gameState = GAME_OVER;
				ambient.stop();
				gameOverMusic.play();
			}

			window.clear();		//Fenster leeren

			player.Update(deltaTime);

			for (int i = 0; i < enemies.size(); i++)
			{
				enemies[i].Update(deltaTime, player.getPosition());
			}

			for (int i = 0; i < groundTiles.size(); i++)
			{
				if (groundTiles[i].second)
					window.draw(groundTiles[i].first);	//die bereits aufgedeckten Bodenkacheln zeichnen 
			}

			for (int j = 0; j < wallTiles.size(); j++)
			{
				if (wallTiles[j].second)				//die bereits aufgedeckten Wandkacheln zeichnen
					window.draw(wallTiles[j].first);
			}

			for (int k = 0; k < decorationTiles.size(); k++)
			{
				if (decorationTiles[k].second)				//die bereits aufgedeckten Dekokacheln zeichnen
					window.draw(decorationTiles[k].first);
			}

			for (int l = 0; l < healthTiles.size(); l++)
			{
				if (healthTiles[l].second)				//die bereits aufgedeckten Dekokacheln zeichnen
					window.draw(healthTiles[l].first);
			}

			if (trapdoor.second)						//falls der Ausgang aufgedeckt wurde, ihn auch zeichnen
				window.draw(trapdoor.first);

			window.draw(spawnpoint);				//Spawnpunkt zeichnen
			player.Draw(window);					//Spieler zeichnen

			window.draw(healthBack);
			window.draw(healthContent);
			window.draw(healthFront);

			for (int i = 0; i < enemies.size(); i++)
			{
				if(enemies[i].isVisible)
					enemies[i].Draw(window);
			}

			window.display();						//Fenster anzeigen
		}
	}

	return 0;
}




