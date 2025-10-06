#include "ZombieArena.h"
#include "Zombie.h"

Zombie* createHorde(int numZombies, sf::IntRect arena)
{
	Zombie* zombies = new Zombie[numZombies];
	int maxY = arena.size.y - 20;
	int minY = 20;
	int maxX = arena.size.x - 20;
	int minX = 20;

	for (int i = 0; i < numZombies; i++)
	{

		// Which side should the Zombie spawn?
		srand((int)time(0) * i);
		int side = rand() % 4;
		float x{}, y{};
		switch (side)
		{
		case 0:
			// spawn left
			x = minX;
			y = (rand() % maxY) + minY;
			break;
		case 1:
			// spawn right
			x = maxX;
			y = (rand() % maxY) + minY;
			break;
		case 2:
			// spawn top
			x = (rand() % maxX) + minX;
			y = minY;
			break;
		case 3:
			// spawn bottom
			x = (rand() % maxX) + minX;
			y = maxY;
			break;
		}

		// Bloater, crawler or runner
		srand((int)time(0)*i*2);
		int type = rand() % 3;
		
		// spawn the new zombie into the array
		zombies[i].spawn(x, y, type, i);

	}

	return zombies;

}