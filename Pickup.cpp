#include "Pickup.h"
#include "TextureHolder.h"
#include <iostream>

Pickup::Pickup(int type)
	: m_Type{ type }
, m_Texture(type == 1 ? TextureHolder::GetTexture("graphics/health_pickup.png") : TextureHolder::GetTexture("graphics/ammo_pickup.png"))
	, m_Sprite(m_Texture)
{
	if (m_Type == 1)
	{
		// m_Texture = TextureHolder::GetTexture("graphics/health_pickup.png");
		// m_Sprite.setTexture(m_Texture);
		// How much is pickup worth
		m_Value = HEALTH_START_VALUE;
	}
	else
	{
		// m_Texture = TextureHolder::GetTexture("graphics/ammo_pickup.png");
		// m_Sprite.setTexture(m_Texture);
		// How much is pickup worth
		m_Value = AMMO_START_VALUE;
	}

	m_Sprite.setOrigin(sf::Vector2f(25, 25));
	m_SecondsToLive = START_SECONDS_TO_LIVE;
	m_SecondsToWait = START_WAIT_TIME;


	// Had to initialize these
	m_Spawned = false;
	m_SecondsSinceSpawn = 0;
	m_SecondsSinceDeSpawn = 0;
}


void Pickup::setArena(sf::IntRect arena)
{
	// Top-edge of the rectangle position
	m_Arena.position.x = arena.position.x + 50;
	m_Arena.position.y = arena.position.y + 50;

	// Avoiding the walls
	m_Arena.size.x = arena.size.x - 50;
	m_Arena.size.y = arena.size.y - 50;

	spawn();
}

void Pickup::spawn()
{
	// spawn at a random location
	srand((int)time(0) / m_Type);
	int x = (rand() % m_Arena.size.x) + m_Arena.position.x;
	srand((int)time(0) * m_Type);
	int y = (rand() % m_Arena.size.y) + m_Arena.position.y;
	m_SecondsSinceSpawn = 0;
	m_Spawned = true;
	m_Sprite.setPosition(sf::Vector2f(x, y));
}

sf::FloatRect Pickup::getPosition()
{
	return m_Sprite.getGlobalBounds();
}

sf::Sprite Pickup::getSprite()
{
	return m_Sprite;
}

bool Pickup::isSpawned() const
{
	return m_Spawned;
}


int Pickup::gotIt()
{
	m_Spawned = false;
	m_SecondsSinceDeSpawn = 0;
	return m_Value;
}

void Pickup::update(float elapsedTime)
{
	std::cout << "Update called - Type: " << m_Type
		<< " Spawned: " << m_Spawned
		<< " SecondsSinceSpawn: " << m_SecondsSinceSpawn
		<< " SecondsSinceDeSpawn: " << m_SecondsSinceDeSpawn << std::endl;

	if (m_Spawned)
	{
		m_SecondsSinceSpawn += elapsedTime;
	}
	else
	{
		m_SecondsSinceDeSpawn += elapsedTime;
	}

	// Do we need to hide a pickup
	if (m_SecondsSinceSpawn > m_SecondsToLive && m_Spawned)
	{
		// Remove the pickup and put it elsewhere
		m_Spawned = false;
		m_SecondsSinceDeSpawn = 0;
	}

	// Do we need to spawn a pickup
	if (m_SecondsSinceDeSpawn > m_SecondsToWait && !m_Spawned)
	{
		// spawn the pickup and reset the timer
		spawn();
	}
}


void Pickup::upgrade()
{
	if (m_Type == 1)
	{
		m_Value += (HEALTH_START_VALUE * .5);
	}
	else
	{
		m_Value += (AMMO_START_VALUE * .5);
	}

	// Make them more frequent and last longer
	m_SecondsToLive += (START_SECONDS_TO_LIVE / 10);
	m_SecondsToWait += (START_WAIT_TIME / 10);
}