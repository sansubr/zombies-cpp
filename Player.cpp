#include "Player.h"
#include <iostream>
#include "TextureHolder.h"

Player::Player()
	: m_Speed(START_SPEED)
	, m_Health(START_HEALTH)
	, m_MaxHealth(START_HEALTH)
	, m_Sprite(TextureHolder::GetTexture("./graphics/player.png"))
{
	/*
	// Associate the texture with the sprite
	if (!m_Texture.loadFromFile("./graphics/player.png"))
	{
		std::cout << "ERROR: Failed to load player.png" << std::endl;
		return;
	}

	std::cout << "Player texture loaded successfully" << std::endl;
	std::cout << "Texture size: " << m_Texture.getSize().x << "x" << m_Texture.getSize().y << std::endl;

	// Reconstruct the sprite with the loaded texture using placement new
	m_Sprite.~Sprite();  // Destroy old sprite
	new (&m_Sprite) sf::Sprite(m_Texture);  // Reconstruct with loaded texture
	*/
	

	// Set the origin of the sprite to the center
	// so that it can rotate smoothly
	sf::FloatRect bounds = m_Sprite.getLocalBounds();
	m_Sprite.setOrigin({ bounds.size.x/2.f, bounds.size.y/2.f });

}


void Player::spawn(sf::IntRect arena, sf::Vector2u resolution, int tileSize)
{
	// Place the player in the middle of the arena
	m_Position.x = arena.size.x / 2.f;
	m_Position.y = arena.size.y / 2.f;

	// Copy the details of the arena 
	// to the player's m_Arena
	m_Arena.position.x = arena.position.x;
	m_Arena.size.x = arena.size.x;
	m_Arena.position.y = arena.position.y;
	m_Arena.size.y = arena.size.y;


	// Remember how big the tiles are in this arena
	m_TileSize = tileSize;


	// Storing the resolution for future use
	m_Resolution.x = resolution.x;
	m_Resolution.y = resolution.y;

	//std::cout << "Player spawned at: " << m_Position.x << ", " << m_Position.y << std::endl;


}



void Player::resetPlayerStats()
{
	m_Speed = START_SPEED;
	m_Health = START_HEALTH;
	m_MaxHealth = START_HEALTH;
}


sf::Time Player::getLastHitTime()
{
	return m_LastHit;
}


bool Player::hit(sf::Time timeHit)
{
	if (timeHit.asMilliseconds() - m_LastHit.asMilliseconds() > 200)
	{
		m_LastHit = timeHit;
		m_Health -= 10;
		return true;
	}
	else
	{
		return false;
	}
}


sf::FloatRect Player::getPosition()
{
	return m_Sprite.getGlobalBounds();
}

sf::Vector2f Player::getCenter()
{
	return m_Position;
}

float Player::getRotation()
{
	return m_Sprite.getRotation().asDegrees();
}

const sf::Sprite& Player::getSprite() const
{
	return m_Sprite;
}

int Player::getHealth() const
{
	return m_Health;
}



void Player::moveLeft()
{
	m_LeftPressed = true;
}

void Player::moveRight()
{
	m_RightPressed = true;
}

void Player::moveUp()
{
	m_UpPressed = true;
}

void Player::moveDown()
{
	m_DownPressed = true;
}

void Player::stopMovingLeft()
{
	m_LeftPressed = false;
}

void Player::stopMovingRight()
{
	m_RightPressed = false;
}

void Player::stopMovingUp()
{
	m_UpPressed = false;
}

void Player::stopMovingDown()
{
	m_DownPressed = false;
}




void Player::update(float elapsedTime, sf::Vector2i mousePosition)
{
	// Player movement
	if (m_UpPressed)
	{
		m_Position.y -= m_Speed * elapsedTime;
	}

	if (m_DownPressed)
	{
		m_Position.y += m_Speed * elapsedTime;
	}

	if (m_RightPressed)
	{
		m_Position.x += m_Speed * elapsedTime;
	}

	if (m_LeftPressed)
	{
		m_Position.x -= m_Speed * elapsedTime;
	}


	


	// Keeping the player in arena bounds

	if (m_Position.x > m_Arena.position.x + m_Arena.size.x - m_TileSize)
	{
		m_Position.x = m_Arena.position.x + m_Arena.size.x - m_TileSize;
	}

	if (m_Position.x < m_Arena.position.x + m_TileSize)
	{
		m_Position.x = m_Arena.position.x + m_TileSize;
	}


	if (m_Position.y > m_Arena.position.y + m_Arena.size.y - m_TileSize)
	{
		m_Position.y = m_Arena.position.y + m_Arena.size.y - m_TileSize;
	}
	
	if (m_Position.y < m_Arena.position.y + m_TileSize)
	{
		m_Position.y = m_Arena.position.y + m_TileSize;
	}


	// Calculating the angle the player is facing
	// Multiplying it by 180 converts this into degrees. Dividing it by Pi, makes the angle range from 0 through 360.
	float radians = atan2(mousePosition.y - m_Resolution.y / 2.f, mousePosition.x - m_Resolution.x / 2.f);
	m_Sprite.setRotation(sf::radians(radians));

	// Updating the sprite position
	m_Sprite.setPosition(m_Position);


	// Debug code
	// std::cout << "Player position: " << m_Position.x << ", " << m_Position.y << std::endl;
	// std::cout << "Sprite position: " << m_Sprite.getPosition().x << ", " << m_Sprite.getPosition().y << std::endl;

}


// 20% speed upgrade
void Player::upgradeSpeed()
{
	m_Speed += START_SPEED * .2;
}

// 20% max health upgrade
void Player::upgradeHealth()
{
	m_MaxHealth += START_HEALTH * .2;
}

// But not beyond max possible health
void Player::increaseHealthLevel(int amount)
{
	m_Health += amount;

	// checking logic for max health
	if (m_Health > m_MaxHealth)
	{
		m_Health = m_MaxHealth;
	}
}
