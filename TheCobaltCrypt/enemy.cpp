#include "enemy.h"



Enemy::Enemy(sf::Texture* texture, sf::Vector2u imageCount, float switchTime, float speed,
	int baseHealth, int baseStrength) : animation(texture, imageCount, switchTime)
{
	this->speed = speed;
	row = 0;
	faceRight = true;
	this->counter = 0;

	this->health = baseHealth;
	this->strength = baseStrength;

	body.setSize(sf::Vector2f(26.0f, 26.0f));					//Feindeskörper
	body.setOrigin(13.0f, 13.0f);
	body.setTexture(texture);

	
}



Enemy::Enemy()
{
	this->speed = 100;
	row = 0;
	faceRight = true;
	this->counter = 0;

	this->health = 110;
	this->strength = 10;

	body.setSize(sf::Vector2f(26.0f, 26.0f));					//Feindeskörper
	body.setOrigin(13.0f, 13.0f);
	
	sf::Texture monsterTex; 
	monsterTex.loadFromFile("monster1.png");
	body.setTexture(&monsterTex);

	this->animation = Animation(&monsterTex, sf::Vector2u(5, 4), 0.2f);
}

Enemy::~Enemy()
{
}

void Enemy::takeDamage(int damageTaken)
{
	health -= damageTaken;
}

int Enemy::dealDamage(int damageDealt)
{
	return damageDealt;
}

void Enemy::Update(float deltaTime, sf::Vector2f playerCoords)
{
	

	if (playerCoords.x > body.getPosition().x)
		faceRight = true;
	else
		faceRight = false;

	

	if (health <= 0 && !dead) {
		die(deltaTime,faceRight);
		timer++;
		if (timer >= 55) // da die Funktion ca. alle 0.15s aufgerufen wird kann man nicht ganz auf 60 gehen,
						//  da sonst wieder die 1. Texur der Todesanimation geladen würde. 
		{
			dead = true;
		}
	}

	if (health > 0 && !attacks) {

		timer = 0;
		body.move(movement);

		if (movement.x == 0 && movement.y == 0) {
			row = 0;
		}
		else
		{
			row = 1;
			if (movement.x > 0.0f)
				faceRight = true;
			else
				faceRight = false;
		}

		animation.Update(row, deltaTime, faceRight);
		body.setTextureRect(animation.uvRect);

		counter++;
		if (counter >= 50)
		{
			if(!followsPlayer)
				direction = rand() % 4 + 1;
			
			counter = 0;
		}
	}
	else
	{
		if (health > 0 && attacks)
		{
			row = 2;
			timer++;
			if (timer > 59)
				attacks = false;
			
			animation.Update(row, deltaTime, faceRight);
			body.setTextureRect(animation.uvRect);

		}
	}
}

void Enemy::Draw(sf::RenderWindow & window)
{
	window.draw(body);
}

void Enemy::SetPosition(sf::Vector2f coords)
{
	body.setPosition(coords);
}


sf::Vector2f Enemy::getPosition()
{
	return body.getPosition();
}

void Enemy::die(float deltaTime, bool faceRight)
{
		animation.Update(3, deltaTime, faceRight);
		body.setTextureRect(animation.uvRect);
	
}

