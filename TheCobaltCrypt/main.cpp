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

Dungeon d(256,144);		// Dungeon d(Breite des Fensters / Texturgröße, Höhe des Fensters / Texturgröße) -> 1280 / 16 && 736 / 16

std::vector<std::pair<sf::RectangleShape, bool> > groundTiles;	//Vektor zum Organisieren der Bodenkacheln, sowie deren Sichtbarkeit
std::vector<std::pair<sf::RectangleShape, bool> > wallTiles;    //Vektor zum Organisieren der Wandkacheln, sowie deren Sichtbarkeit
std::vector<std::pair<sf::RectangleShape, bool> > decorationTiles;
std::vector<std::pair<sf::RectangleShape, bool> > healthTiles;
std::vector<std::pair<sf::RectangleShape, bool> > armorTiles;
std::vector<Enemy> enemies;

int rooms;
int blueDur = 15;
int misc = BLUE;
int attackTimer = 0;
int switchViews = 0;

std::vector<std::vector<int> > map;												//Array um festzustellen, wo welche Bodenkachel liegt
std::vector<std::vector<int> > mapWalls;										//Array um festzustellen, wo welche Wandkachel liegt
std::vector<std::vector<int> > mapDecorations;
std::vector<std::vector<int> > mapHealthPickups;
std::vector<std::vector<int> > mapArmorPickups;

sf::Texture main_menu[2];
sf::Texture game_over[2];
sf::Sprite mmText[2];
sf::Sprite goText[2];

sf::Sprite mmBackground;
sf::Sprite goBackground;
sf::Sprite healthFront;
sf::Sprite healthBack;
sf::Sprite healthContent;
sf::Sprite armorFront;
sf::Sprite armorBack;
sf::Sprite armorContent;

sf::Texture wallTextures[6];									//Wandtextur
sf::Texture groundTextures[6];									//Bodentextur
sf::Texture decorations[34];
sf::Texture monsters[6];

sf::FloatRect visibleArea(0, 0, 0, 0);

sf::Texture t_healthBack;
sf::Texture t_healthFront;
sf::Texture t_healthContent;
sf::Texture t_healthPickup;
sf::Texture t_armorBack;
sf::Texture t_armorFront;
sf::Texture t_armorContent;
sf::Texture t_armorPickup;

sf::View view1(sf::FloatRect(0.0f, 0.0f, 1280.0f, 720.0f));

sf::FloatRect normalSize(0.0f, 0.0f, 4096.0f, 2304.0f);

sf::RectangleShape spawnpoint(sf::Vector2f(16.0f, 16.0f));		//Rechteck, welches den Spawnpunkt markiert. Immer sichtbar

std::pair<int, int> spawnChoords;				//Pair, welches die Startkoordinaten enthält
std::pair<sf::RectangleShape, bool> trapdoor;	//Pair, welches den rechteckigen Ausgang, sowie dessen Sichtbarkeit regelt

std::random_device radev;	
std::mt19937 eng(radev());
std::uniform_int_distribution<> distr(160, 400);
std::uniform_int_distribution<> rnd(0, 5);
std::uniform_int_distribution<> rndDeco(0, 33);

sf::Music menuMusic;
sf::Music gameOverMusic;

sf::Sound s_heal;
sf::Sound s_armor;
sf::Sound s_hit;
sf::Sound s_hitUnarmored;

sf::Vector2f dimensions;


void resetTextures() {

	map.clear();						//die Vektoren "leeren"
	mapWalls.clear();
	mapDecorations.clear();
	mapHealthPickups.clear();
	mapArmorPickups.clear();

	enemies.reserve(div(rooms,5).quot);

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
	for (int j = 0; j < healthTiles.size(); j++)
	{
		healthTiles[j].first.setTexture(&t_healthPickup);
		healthTiles[j].second = false;
		healthTiles[j].first.setSize(TEXTURE_SIZE);
	}
	for (int j = 0; j < armorTiles.size(); j++)
	{
		armorTiles[j].first.setTexture(&t_armorPickup);
		armorTiles[j].second = false;
		armorTiles[j].first.setSize(TEXTURE_SIZE);
	}

	for (int l = 0; l < div(rooms,5).quot; l++) {
		Enemy x(&monsters[rnd(eng)], sf::Vector2u(5, 4), 0.2f, 100.0f, 110, 10);
		enemies.push_back(x);
		enemies[l].isVisible = false;
	}
	healthContent.setScale(1.5f, 1.5f);
}

void prepareTextures() {
	groundTiles.resize(d.countFloorTiles());	//Boden- und Wandkachelvektor auf die der Anzahl an Kacheln entsprechende Größe bringen
	wallTiles.resize(d.countWallTiles());
	decorationTiles.resize(d.countDecoTiles());
	healthTiles.resize(d.countPickupTiles().first);
	armorTiles.resize(d.countPickupTiles().second);
	enemies.reserve(div(rooms,5).quot);

	
	t_healthBack.loadFromFile("images/lifebar_back.png");
	t_healthFront.loadFromFile("images/lifebar_front.png");
	t_healthContent.loadFromFile("images/lifebar_content.png");
	t_healthPickup.loadFromFile("images/heal.png");

	t_armorBack.loadFromFile("images/armorbar_back.png");
	t_armorFront.loadFromFile("images/armorbar_front.png");
	t_armorContent.loadFromFile("images/armorbar_content.png");
	t_armorPickup.loadFromFile("images/armor.png");


	healthBack.setTexture(t_healthBack);
	healthFront.setTexture(t_healthFront);
	healthContent.setTexture(t_healthContent);

	armorBack.setTexture(t_armorBack);
	armorFront.setTexture(t_armorFront);
	armorContent.setTexture(t_armorContent);

	healthBack.setPosition(0.0f, 0.0f);
	healthFront.setPosition(0.0f, 0.0f);
	healthContent.setPosition(21.0f, 16.5f);

	armorBack.setPosition(0.0f, 30.0f);
	armorFront.setPosition(0.0f, 30.0f);
	armorContent.setPosition(21.0f, 46.5f);

	healthBack.setScale(1.5f, 1.5f);
	healthFront.setScale(1.5f, 1.5f);
	healthContent.setScale(1.5f, 1.5f);

	armorBack.setScale(1.5f, 1.5f);
	armorFront.setScale(1.5f, 1.5f);
	armorContent.setScale(1.5f, 1.5f);

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

	for (int l = 0; l < div(rooms,5).quot; l++)	{
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

	for (int n = 0; n < armorTiles.size(); n++)
	{
		armorTiles[n].first.setTexture(&t_armorPickup);
		armorTiles[n].second = false;
		armorTiles[n].first.setSize(TEXTURE_SIZE);
	}
}

int processMainMenu(sf::RenderWindow& window) {
	view1.setCenter(640.0f, 360.0f);
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
	view1.setCenter(640.0f, 360.0f);
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

sf::View zoomViewAt(sf::Vector2i pixel, sf::RenderWindow& window, float zoom)
{
	const sf::Vector2f beforeCoord{ window.mapPixelToCoords(pixel) };
	sf::View view{ window.getView() };
	view.zoom(zoom);
	window.setView(view);
	const sf::Vector2f afterCoord{ window.mapPixelToCoords(pixel) };
	const sf::Vector2f offsetCoords{ beforeCoord - afterCoord };
	view.move(offsetCoords);
	window.setView(view);
	return view;
}

//_________________Erzeugen___des___Dungeons_________________//
void generateDungeon() {
	
	rooms = distr(eng);  //Erzeugen einer Zufallszahl zwischen 160 und 400
	d.generate(rooms);		//Erzeugen des Dungeons mit der generierten Zahl
	std::cout << "Abgeschlossen. \n Dungeon sollte jetzt " << rooms << " Raeume und Korridore haben." << std::endl;
	//d.print(); //Ausgabe in Konsole (nur für Debugging mit kleineren Werten -> sonst unlesbar)

}

bool isWall(int offsetX, int offsetY, Player player) {

	if (d.getTile((player.getPosition().x + offsetX) / 16, (player.getPosition().y + offsetY) / 16) == '1'
		|| d.getTile((player.getPosition().x + offsetX) / 16, (player.getPosition().y + offsetY) / 16) == '4'
		|| d.getTile((player.getPosition().x + offsetX) / 16, (player.getPosition().y + offsetY) / 16) == '5'
		|| d.getTile((player.getPosition().x + offsetX) / 16, (player.getPosition().y + offsetY) / 16) == '6'
		|| d.getTile((player.getPosition().x + offsetX) / 16, (player.getPosition().y + offsetY) / 16) == '7'
		|| d.getTile((player.getPosition().x + offsetX) / 16, (player.getPosition().y + offsetY) / 16) == '8') //angrenzende Kachel überprüfen (da man nicht durch Wände gehen soll)
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
	int armor = 0;

	for (int y = 0; y < 144; y++) {
		for (int x = 0; x < 256; x++)
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
						mapHealthPickups[n][m] = health;							//auch hier müssen 256 Plätze reserviert werden
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
				trapdoor.second = true;								//Ausgang ist am Levelbeginn nicht sichtbar
				break;
			case '8':
				armorTiles[armor].first.setPosition(x * 16, y * 16);
				for (int m = y * 16; m < (y + 1) * 16; m++)
				{
					for (int n = x * 16; n < (x + 1) * 16; n++)
					{
						mapArmorPickups[n][m] = armor;							//auch hier müssen 256 Plätze reserviert werden
					}
				}
				armor++;
				break;
			}
		}
	}

}




int main() {

	sf::RenderWindow window(sf::VideoMode(1280, 720), "The Cobalt Crypt");	//Spielfenster anlegen

	window.setFramerateLimit(FRAMERATE);
	int gameState = 0;

	float deltaTime = 0.0f;
	sf::Clock clock;

	std::cout << "Lade Texturen..." << std::endl;
	
	sf::Texture playerTexture;
	playerTexture.loadFromFile("images/warrior.png");

	Player player(&playerTexture, sf::Vector2u(10, 10), 0.1f, 100.0f,100, 20,0);

	
	sf::Texture bg;
	bg.loadFromFile("images/Main Menu.png");
	mmBackground.setTexture(bg);
	
	sf::Texture gameOverBg;
	gameOverBg.loadFromFile("images/GameOver.png");
	goBackground.setTexture(gameOverBg);


	for (int i = 0; i < 2; i++)
	{
		main_menu[i].loadFromFile("images/text" + std::to_string(i + 2) + ".png");
		mmText[i].setTexture(main_menu[i]);
	}

	for (int i = 0; i < 2; i++)
	{
		game_over[i].loadFromFile("images/GOtext" + std::to_string(i) + ".png");
		goText[i].setTexture(game_over[i]);
	}

										
	for (int i = 0; i < 6; i++)
	{

		groundTextures[i].loadFromFile("images/floor_var" + std::to_string(i + 1) + ".png");
	}


	for (int i = 0; i < 6; i++)
	{

		wallTextures[i].loadFromFile("images/wall_var" + std::to_string(i + 1) + ".png");
	}

	
	
	for (int i = 0; i < 34; i++)
	{
		
		decorations[i].loadFromFile("images/decoration" + std::to_string(i+1) + ".png");
	}

	
	for (int i = 0; i < 6; i++)
	{

		monsters[i].loadFromFile("images/monster" + std::to_string(i + 1) + ".png");
	}
	

	sf::Texture exitTile;													//Textur des Ausgangs
	exitTile.loadFromFile("images/exitTile.png");

	sf::Texture spawnTile;													//Textur des Spawnpunktes
	spawnTile.loadFromFile("images/spawn.png");

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

	
	sf::SoundBuffer healSound;
	healSound.loadFromFile("heal.wav");

	sf::SoundBuffer armorSound;
	armorSound.loadFromFile("armor.wav");

	sf::SoundBuffer hitSound;
	hitSound.loadFromFile("damage.wav");

	sf::SoundBuffer hitUnarmoredSound;
	hitUnarmoredSound.loadFromFile("damage_unarmored.wav");

	s_heal.setBuffer(healSound);
	s_armor.setBuffer(armorSound);
	s_hit.setBuffer(hitSound);
	s_hitUnarmored.setBuffer(hitUnarmoredSound);
	
	bool playing = true;
	bool menuPlaying = true;
	bool gameOverPlaying = false;
	bool checkedOnce = true;
	bool moving = false;

	sf::Vector2f oldPos;

	dimensions.x = 1280.0f;
	dimensions.y = 720.0f;

	int distanceToPlayerX;
	int distanceToPlayerY;
//________________________SPIELBEGINN___________________________

	while (window.isOpen())			//während das Fenster geöffnet ist
	{
			window.setView(view1);

		deltaTime = clock.restart().asSeconds();

		window.clear();				//alles löschen

		switch (gameState)
		{
		case MAIN_MENU:
			menuPlaying = true;
			window.setView(view1);
			view1.setSize(1280.0f, 720.0f);
			view1.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));
			window.draw(mmBackground);
			gameState = processMainMenu(window);
			
			checkedOnce = true;
			break;
		case GAME_OVER:
			gameOverPlaying = true;
			window.setView(view1);
			window.draw(goBackground);
			view1.setSize(1280.0f, 720.0f);
			view1.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));
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
				dimensions.x = evnt.size.width;
				dimensions.y = evnt.size.height;
				break;
			case sf::Event::MouseButtonPressed:
				// Mouse button is pressed, get the position and set moving as active
				if (evnt.mouseButton.button == 0) {
					moving = true;
					oldPos = window.mapPixelToCoords(sf::Vector2i(evnt.mouseButton.x, evnt.mouseButton.y));
				}
				break;
			case  sf::Event::MouseButtonReleased:
				// Mouse button is released, no longer move
				if (evnt.mouseButton.button == 0) {
					moving = false;
				}
				break;
			case sf::Event::MouseMoved:
			{
				if (gameState == GAME) {
					// Ignore mouse movement unless a button is pressed (see above)
					if (!moving)
						break;
					// Determine the new position in world coordinates
					const sf::Vector2f newPos = window.mapPixelToCoords(sf::Vector2i(evnt.mouseMove.x, evnt.mouseMove.y));
					// Determine how the cursor has moved
					// Swap these to invert the movement direction
					const sf::Vector2f deltaPos = oldPos - newPos;

					// Move our view accordingly and update the window
					view1.setCenter(view1.getCenter() + deltaPos);
					window.setView(view1);

					// Save the new position as the old one
					// We're recalculating this, since we've changed the view
					oldPos = window.mapPixelToCoords(sf::Vector2i(evnt.mouseMove.x, evnt.mouseMove.y));
					break;
				}
			}
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

			case sf::Event::MouseWheelScrolled:
				if (gameState == GAME) {
					if (evnt.mouseWheelScroll.delta > 0)
						view1 = zoomViewAt({ evnt.mouseWheelScroll.x, evnt.mouseWheelScroll.y }, window, (1.f / 1.1f));
					else if (evnt.mouseWheelScroll.delta < 0)
						view1 = zoomViewAt({ evnt.mouseWheelScroll.x, evnt.mouseWheelScroll.y }, window, 1.1f);

				}
			}
			
		}

	
		if (gameState == GAME) {

		//	view1.setSize(dimensions);
			menuPlaying = false;

			if (checkedOnce)
			{
				std::cout << "Generiere Dungeon..." << std::endl;
				d.clearDungeon(256, 144);
				generateDungeon();							//Dungeon generieren
				prepareTextures();

				player.resurrect();
				
				

				map.clear();
				mapWalls.clear();
				mapDecorations.clear();
				mapHealthPickups.clear();
				mapArmorPickups.clear();

				std::vector<int> row(2304, 0);

				for (int i = 0; i < 4096; i++)
				{
					map.push_back(row);
					mapWalls.push_back(row);
					mapDecorations.push_back(row);
					mapHealthPickups.push_back(row);
					mapArmorPickups.push_back(row);
				}

				


				showDungeon();		//Dungeon für SFML verwendbar machen

				player.SetPosition(sf::Vector2f(spawnChoords.first, spawnChoords.second));	//Spieler auf den Spawnpunkt setzen
				view1.setCenter(player.getPosition());

				int posX = rand() % 4096;
				int posY = rand() % 2304;

				for (int l = 0; l < enemies.size(); l++)
				{
					while (map[posX][posY] == 0)
					{
						posX = rand() % 4096;
						posY = rand() % 2304;
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
					view1.setCenter(player.getPosition());

				}		
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) { //s.o. (für "D")
				if (!isWall(4, 2, player))
				{
					player.movePlayer(sf::Vector2f(100 * deltaTime, 0.0f));
					view1.setCenter(player.getPosition());

				}
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) { //s.o. (für "W")
				if (!isWall(0, 0, player))
				{
					player.movePlayer(sf::Vector2f(0.0f, -100 * deltaTime)); 
					view1.setCenter(player.getPosition());

				}
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {  //s.o. (für "S")
				if (!isWall(0, 5, player))
				{
					player.movePlayer(sf::Vector2f(0.0f, 100 * deltaTime));
					view1.setCenter(player.getPosition());

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
			
			healthTiles[mapHealthPickups[(int)player.getPosition().x][(int)player.getPosition().y]].second = true;
			healthTiles[mapHealthPickups[(int)player.getPosition().x + 16][(int)player.getPosition().y]].second = true;
			healthTiles[mapHealthPickups[(int)player.getPosition().x - 16][(int)player.getPosition().y]].second = true;
			healthTiles[mapHealthPickups[(int)player.getPosition().x][(int)player.getPosition().y + 16]].second = true;
			healthTiles[mapHealthPickups[(int)player.getPosition().x][(int)player.getPosition().y - 16]].second = true;

			armorTiles[mapArmorPickups[(int)player.getPosition().x][(int)player.getPosition().y]].second = true;
			armorTiles[mapArmorPickups[(int)player.getPosition().x + 16][(int)player.getPosition().y]].second = true;
			armorTiles[mapArmorPickups[(int)player.getPosition().x - 16][(int)player.getPosition().y]].second = true;
			armorTiles[mapArmorPickups[(int)player.getPosition().x][(int)player.getPosition().y + 16]].second = true;
			armorTiles[mapArmorPickups[(int)player.getPosition().x][(int)player.getPosition().y - 16]].second = true;

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
				&& healthTiles[mapHealthPickups[(int)player.getPosition().x][(int)player.getPosition().y]].first.getTexture() != &groundTextures[4]) {
				player.pickup(1);
				s_heal.play();
				healthTiles[mapHealthPickups[(int)player.getPosition().x][(int)player.getPosition().y]].first.setTexture(&groundTextures[4]);
			}
			else if (d.getTile(player.getPosition().x / 16, (player.getPosition().y) / 16) == '8'
				&& armorTiles[mapArmorPickups[(int)player.getPosition().x][(int)player.getPosition().y]].first.getTexture() != &groundTextures[4])
			{
				player.pickup(2);
				s_armor.play();
				armorTiles[mapArmorPickups[(int)player.getPosition().x][(int)player.getPosition().y]].first.setTexture(&groundTextures[4]);
			}

			sf::Vector2f movement(0.0f, 0.0f);

			

			for (int i = 0; i < enemies.size(); i++) {
				
				distanceToPlayerX = player.getPosition().x - enemies[i].getPosition().x;
				distanceToPlayerY = player.getPosition().y - enemies[i].getPosition().y;
				enemies[i].movement = movement;

				if ((distanceToPlayerX > -65 && distanceToPlayerX < 65) && (distanceToPlayerY > -65 && distanceToPlayerY < 65)) {
					std::cout << "Gegner " << i << " jagt Spieler" << std::endl;
				}
				else
				{
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
				}

				if (groundTiles[map[(int)enemies[i].getPosition().x][(int)enemies[i].getPosition().y]].second && !enemies[i].dead)
					enemies[i].isVisible = true;
				else
				{
					enemies[i].isVisible = false;
				}

				if ((distanceToPlayerX > -25 && distanceToPlayerX < 25) && (distanceToPlayerY > -25 && distanceToPlayerY < 25))
				{
					if (!enemies[i].dead) {
						enemies[i].attacks = true;
						attackTimer++;
						if (attackTimer == 62) {
							player.takeDamage(enemies[i].dealDamage(enemies[i].strength));
							
							if (player.getArmor() <= 0)
								s_hitUnarmored.play();
							else
								s_hit.play();
							
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

			armorContent.setScale(player.getArmor() * 1.5f / player.maxArmor, 1.5f);
			armorContent.setPosition(21 + ((player.getArmor() * 1.5f / player.maxArmor) * (-6.6667f) + 11), 46.5f);

			
			
			if (d.getTile(player.getPosition().x / 16, (player.getPosition().y) / 16) == '7') { //Erreicht der Spieler den Ausgang

				d.clearDungeon(256, 144);						//Dungeon löschen
				generateDungeon();							//neuen Dungeon erzeugen

				groundTiles.resize(d.countFloorTiles());	//die drei Vektoren auf die richtige Größe bringen
				wallTiles.resize(d.countWallTiles());
				decorationTiles.resize(d.countDecoTiles());
				healthTiles.resize(d.countPickupTiles().first);
				armorTiles.resize(d.countPickupTiles().second);
				enemies.clear();
				
				resetTextures();

				std::vector<int> row(2304, 0);

				for (int i = 0; i < 4096; i++)
				{
					map.push_back(row);
					mapWalls.push_back(row);
					mapDecorations.push_back(row);
					mapHealthPickups.push_back(row);
					mapArmorPickups.push_back(row);
				}


				//prepareTextures();
				showDungeon();			//Dungeon für SFML vorbereiten

				player.SetPosition(sf::Vector2f(spawnChoords.first, spawnChoords.second));	//Spieler auf Spawnpunkt setzen
				
				int posX = rand() % 4096;
				int posY = rand() % 2034;

				for (int l = 0; l < enemies.size(); l++)
				{
					while (map[posX][posY] == 0)
					{
						posX = rand() % 4096;
						posY = rand() % 2304;
					}

					enemies[l].SetPosition(sf::Vector2f(posX, posY));
					posX = 0;
					posY = 0;
				}     
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

			if (switchViews == 0) {

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

				for (int m = 0; m < armorTiles.size(); m++)
				{
					if (armorTiles[m].second)				//die bereits aufgedeckten Dekokacheln zeichnen
						window.draw(armorTiles[m].first);
				}

				if (trapdoor.second)						//falls der Ausgang aufgedeckt wurde, ihn auch zeichnen
					window.draw(trapdoor.first);

				for (int i = 0; i < enemies.size(); i++)
				{
					if (enemies[i].isVisible)
						enemies[i].Draw(window);
				}

				window.draw(spawnpoint);				//Spawnpunkt zeichnen
				player.Draw(window);					//Spieler zeichnen
			}
			
			window.setView(window.getDefaultView());
			window.draw(healthBack);
			window.draw(healthContent);
			window.draw(healthFront);
			
			window.draw(armorBack);
			window.draw(armorContent);
			window.draw(armorFront);

			
			window.display();						//Fenster anzeigen
		}
	}

	return 0;
}




