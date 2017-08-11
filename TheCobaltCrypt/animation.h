#pragma once

#include <SFML\Graphics.hpp>

class Animation
{
public:
	Animation(sf::Texture* texture, sf::Vector2u imageCount, float switchTime);

	Animation();

	Animation(const Animation&) = default;

	~Animation();

	void Update(int row, float deltaTime, bool faceRight);

public:
	sf::IntRect uvRect;

private:
	sf::Vector2u imageCount;
	sf::Vector2u currentImage;
	sf::Vector2u lastImage = sf::Vector2u(0, 0);

	float totalTime;
	float switchTime;
};

