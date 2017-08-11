#pragma once
#include <SFML\Graphics.hpp>
#include <stdio.h>      
#include <stdlib.h>
#include "animation.h"


class Enemy
{
public:
	Enemy(sf::Texture* texture, sf::Vector2u imageCount, float switchTime, float speed,
		int baseHealth, int baseStrength);

	Enemy(const Enemy&) = default;

	Enemy(Enemy&&) noexcept {}

	Enemy();

	~Enemy();

	void takeDamage(int damageTaken);

	int dealDamage(int damageDealt);

	void Update(float deltaTime, sf::Vector2f playerCoords);

	void Draw(sf::RenderWindow& window);

	void SetPosition(sf::Vector2f coords);

	sf::Vector2f getPosition();

	void die(float deltaTime, bool faceRight);




public:
	int direction;
	int counter;
	bool isVisible;
	bool dead = false;
	bool attacks = false;
	int timer = 0;
	int strength;
	sf::Vector2f movement;


private:

	sf::RectangleShape body;
	Animation animation;
	unsigned int row;
	float speed;
	bool faceRight;
	int health;


};

