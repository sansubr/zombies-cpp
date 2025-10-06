#pragma once
#include <SFML/Graphics.hpp>

class Player
{
private:
	const float START_SPEED = 200;
	const float START_HEALTH = 100;

	// Where is the player?
	sf::Vector2f m_Position;

	
	// The sprite and texture
	sf::Texture m_Texture;
	sf::Sprite m_Sprite;


	// What is the current resolution?
	sf::Vector2u m_Resolution;

	
	// What is the size of the arena?
	sf::IntRect m_Arena;

	// How big is each tile of the arena
	int m_TileSize;


	// Which direction is the player moving in?
	bool m_UpPressed = false;
	bool m_DownPressed = false;
	bool m_LeftPressed = false;
	bool m_RightPressed = false;


	// How much health has the player got?
	int m_Health;

	// What is the max health a player can have?
	int m_MaxHealth;


	// When was the player hit last?
	sf::Time m_LastHit;


	// Speed in pixels per second
	float m_Speed;


public:

	// Default constructor looks like
	Player();

	
	// Spawning the player?
	void spawn(sf::IntRect arena, sf::Vector2u resolution, int tileSize);

	// Call this at the end of every game
	void resetPlayerStats();

	// Handle the player getting hit by a Zombie
	bool hit(sf::Time timeHit);

	// How long ago was the player last hit?
	sf::Time getLastHitTime();

	// Where is the player?
	sf::FloatRect getPosition();

	// Where is the center of the player?
	sf::Vector2f getCenter();

	// What angle is the player facing?
	float getRotation();

	// Send a copy of the Sprite to the main function
	const sf::Sprite& getSprite() const;

	// These move the player
	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();

	// Stop the player moving in a specific direction
	void stopMovingLeft();
	void stopMovingRight();
	void stopMovingUp();
	void stopMovingDown();


	// We will call this function once every frame
	void update(float elapsedTime, sf::Vector2i mousePosition);


	// Give player a speed boost
	void upgradeSpeed();


	// Give player some health
	void upgradeHealth();


	// Increase the max health a player can have
	void increaseHealthLevel(int amount);


	// How much health has the player currently got?
	int getHealth() const;





};