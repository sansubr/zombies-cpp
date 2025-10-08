#include "zombie.h"
#include "TextureHolder.h"

#include <cstdlib>
#include <ctime>

Zombie::Zombie()
	: m_Speed(0)
	, m_Health(0)
	, m_Alive(false)
	, m_Texture(nullptr)
	, m_Sprite(std::nullopt)
{ }

void Zombie::spawn(float startX, float startY, int type, int seed)
{
	m_Alive = true;

	switch (type)
	{
	case 0:
		// Bloater
		m_Texture = &TextureHolder::GetTexture("./graphics/bloater.png");
		m_Speed = BLOATER_SPEED;
		m_Health = BLOATER_HEALTH;
		break;
	case 1:
		// Chaser
		m_Texture = &TextureHolder::GetTexture("./graphics/chaser.png");
		m_Speed = CHASER_SPEED;
		m_Health = CHASER_HEALTH;
		break;
	case 2:
		// Crawler
		m_Texture = &TextureHolder::GetTexture("./graphics/crawler.png");
		m_Speed = CRAWLER_SPEED;
		m_Health = CRAWLER_HEALTH;
		break;
	} // end of switch

	// Reconstructing the sprite
	m_Sprite.emplace(*m_Texture);


	// Modify the speed to make the zombie unique
	// Every zombie is unique with a speed modifier
	srand((int)time(0) * seed);

	// Speed modifier somewhere between 80 and 100
	float modifier = (rand() % MAX_VARIANCE) + OFFSET;


	// Express this as a fraction of 1
	modifier /= 100; // Equals between .7 and 1
	m_Speed *= modifier;


	// Initialize its location
	m_Position.x = startX;
	m_Position.y = startY;

	// set its origin to its center
	m_Sprite->setOrigin(sf::Vector2f(25, 25));


	// set its position
	m_Sprite->setPosition(m_Position);

} // End of spawn


bool Zombie::hit()
{
	m_Health--;
	if (m_Health < 0)
	{
		// zombie dead
		m_Alive = false;
		const sf::Texture* bloodTexture = &TextureHolder::GetTexture("graphics/blood.png");
		m_Sprite.emplace(*bloodTexture);
		m_Sprite->setOrigin(sf::Vector2f(25, 25));
		m_Sprite->setPosition(m_Position);
		return true;
	}
	// Else return false
	// Because zombie is injured, but not dead yet
	return false;
}


bool Zombie::isAlive() const
{
	return m_Alive;
}

sf::FloatRect Zombie::getPosition()
{
	
	if (m_Sprite.has_value())
	{
		return m_Sprite->getGlobalBounds();
	}
	else
	{
		return sf::FloatRect({ 0,0 }, { 0,0 });
	}
	
}

const sf::Sprite& Zombie::getSprite() const
{
	return *m_Sprite;
}


void Zombie::update(float elapsedTime, sf::Vector2f playerLocation)
{

	float playerX = playerLocation.x;
	float playerY = playerLocation.y;

	// Update the zombie's position variables
	if (playerX > m_Position.x)
	{
		m_Position.x = m_Position.x + m_Speed * elapsedTime;
	}
	
	if (playerY > m_Position.y)
	{
		m_Position.y = m_Position.y + m_Speed * elapsedTime;
	}

	if (playerX < m_Position.x)
	{
		m_Position.x = m_Position.x - m_Speed * elapsedTime;
	}
	
	if (playerY < m_Position.y)
	{
		m_Position.y = m_Position.y - m_Speed * elapsedTime;
	}

	// Move the sprite
	m_Sprite->setPosition(m_Position);

	// Face the sprite in the right direction
	m_Sprite->setRotation(sf::radians(atan2(playerY - m_Position.y, playerX - m_Position.x)));
}