#pragma once
#include <SFML/Graphics.hpp>

class Zombie
{

private:

	// How fast is each zombie type?
	static constexpr float BLOATER_SPEED = 40;
	static constexpr float CHASER_SPEED = 80;
	static constexpr float CRAWLER_SPEED = 20;

	// How tough is each zombie?
	static constexpr float BLOATER_HEALTH = 5;
	static constexpr float CHASER_HEALTH = 1;
	static constexpr float CRAWLER_HEALTH = 3;

	// Making each zombie vary its speed slightly
	static const int MAX_VARIANCE = 30;
	static const int OFFSET = 101 - MAX_VARIANCE;


	// Where is this Zombie?
	sf::Vector2f m_Position;


	// A texture for the zombie
	sf::Texture m_Texture;


	// A sprite for the zombie
	sf::Sprite m_Sprite;


	// How fast can this one run or crawl?
	float m_Speed;

	// How much health has it got?
	float m_Health;

	// Is it still alive?
	bool m_Alive;

public:

	Zombie();

	// handle when bullet hits a Zombie
	bool hit();


	// Find out if the Zombie is alive
	bool isAlive() const;


	// Spawn a new Zombie
	void spawn(float startX, float startY, int type, int seed);


	// Return a rectangle that is the position in the world
	sf::FloatRect getPosition();


	// Get a copy of the sprite to draw
	const sf::Sprite& getSprite() const;


	// Update the Zombie each frame
	void update(float elapsedTime, sf::Vector2f playerLocation);

};
