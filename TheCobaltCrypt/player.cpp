#include "player.h"

Player::Player(sf::Texture* texture, sf::Vector2u imageCount, float switchTime, float speed,
	int baseHealth, int baseStrength, int points) 
	: animation(texture, imageCount, switchTime)
{
	this->speed = speed;
	row = 0;
	faceRight = true;

	this->health = baseHealth;
	this->strength = baseStrength;
	this->armor = maxArmor;

	body.setSize(sf::Vector2f(19.0f, 32.0f));					//Spieler
	body.setOrigin(9.5f, 20.0f);								//da sonst die Koordinaten des Spielers von der oberen linken Ecke gelesen werden würden,
																//Mittelpunkt festlegen
	body.setTexture(texture);									//... und Textur verwenden

}


int Player::getHealth() {
	return health;
}

int Player::getArmor()
{
	return armor;
}

void Player::pickup(int item) {
	
	switch (item)
	{
	case 1:
		health += 20;

		if (health > maxHealth)
			health = maxHealth;
		break;
	case 2:
		armor += 5;

		if (armor > maxArmor)
			armor = maxArmor;
		break;

	default:
		break;
	}
	
}

void Player::takeDamage(int damageTaken) {
	if(armor >= damageTaken)
		armor-= damageTaken;
	else {
		damageTaken -= armor;
		armor = 0;
		health -= damageTaken;
	}
}

int Player::dealDamage() {
	return strength;
}

void Player::Update(float deltaTime)
{
	sf::Vector2f movement(0.0f, 0.0f);
	
	if (health <= 0 && !dead) {
		die(deltaTime, faceRight);
		deadnessTimer++;
		if (deadnessTimer >= 59) // da die Funktion ca. alle 0.15s aufgerufen wird kann man nicht ganz auf 60 gehen,
						 //  da sonst wieder die 1. Texur der Todesanimation geladen würde. 
		{
			dead = true;
		}
	}

	if (health > 0) {

		deadnessTimer = 0;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			movement.x -= speed * deltaTime;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			movement.x += speed * deltaTime;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			movement.y -= speed * deltaTime;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			movement.y -= speed * deltaTime;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			attacks = true;
			timer = 0;
		}

		if (movement.x == 0 && movement.y == 0 && !attacks) {
			row = 0;
		}
		else if (attacks)
		{
			row = 3;

			if (movement.x > 0.0f)
				faceRight = true;
			else if (movement.x < 0.0f) //verhindert das Umdrehen beim Angriff wenn nicht "D" gedrückt wird
				faceRight = false;

			if (timer > 59)
				attacks = false;

		}
		else
		{
			row = 2;

			if (movement.x > 0.0f)
				faceRight = true;
			else
				faceRight = false;

		}
		timer++;

		animation.Update(row, deltaTime, faceRight);
		body.setTextureRect(animation.uvRect);
	}
}

void Player::Draw(sf::RenderWindow& window)
{
	window.draw(body);
}

void Player::SetPosition(sf::Vector2f coords)
{
	body.setPosition(coords);
}

void Player::movePlayer(sf::Vector2f movement)
{
	body.move(movement);
}

sf::Vector2f Player::getPosition()
{
	return body.getPosition();
}

void Player::die(float deltaTime, bool faceRight)
{
	animation.Update(4, deltaTime, faceRight);
	body.setTextureRect(animation.uvRect);

}

void Player::resurrect()
{
	health = 100;
	dead = false;
}

