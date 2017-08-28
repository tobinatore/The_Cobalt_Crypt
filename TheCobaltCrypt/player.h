#pragma once

#include "animation.h"

class Player
{
public:
	int points = 0;
	bool attacks = false;
	int maxHealth = 100;
	int maxArmor = 100;

public:
	Player(sf::Texture* texture, sf::Vector2u imageCount, float switchTime, float speed,
		int baseHealth, int baseStrength, int points);

	int getHealth();

	int getArmor();

	void pickup(int item);

	void takeDamage(int damageTaken);

	int dealDamage();

	void Update(float deltaTime);

	void Draw(sf::RenderWindow& window);

	void SetPosition(sf::Vector2f coords);

	void movePlayer(sf::Vector2f movement);
	
	sf::Vector2f getPosition();

	void die(float deltaTime, bool faceRight);
	
	void resurrect();

	bool dead = false;

private:
	sf::RectangleShape body;
	Animation animation;
	unsigned int row;
	float speed;
	bool faceRight;
	int health;
	int armor;
	int strength;
	int timer = 10;
	int deadnessTimer = 0;
	
};