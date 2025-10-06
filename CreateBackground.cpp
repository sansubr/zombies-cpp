#include "ZombieArena.h"

int createBackground(sf::VertexArray& rVA, sf::IntRect arena)
{

	// Anything we do to rVA, we are really doing
	// to the background (in the main function)

	// How big is each tile/texture

	const int TILE_SIZE = 50;
	const int TILE_TYPES = 3;
	const int VERTS_IN_QUAD = 6;
	int worldWidth = arena.size.x / TILE_SIZE;
	int worldHeight = arena.size.y / TILE_SIZE;

	// What primitive are we using?
	rVA.setPrimitiveType(sf::PrimitiveType::Triangles);

	// Set the size of the vertex array
	rVA.resize(static_cast<size_t>(worldWidth) * worldHeight * VERTS_IN_QUAD);

	// Start at the beginning of the vertex array
	int currentVertex = 0;

	for (int w = 0; w < worldWidth; w++)
	{
		for (int h = 0; h < worldHeight; h++)
		{

			// Define the four sides of the Quad first
			sf::Vector2f topLeft(w * TILE_SIZE, h * TILE_SIZE);
			sf::Vector2f topRight((w * TILE_SIZE) + TILE_SIZE, h * TILE_SIZE);
			sf::Vector2f bottomRight((w * TILE_SIZE) + TILE_SIZE, (h * TILE_SIZE) + TILE_SIZE);
			sf::Vector2f bottomLeft(w * TILE_SIZE, (h * TILE_SIZE) + TILE_SIZE);


			// Assign each side to a triangle vertex
			// First triangle
			rVA[static_cast<size_t>(currentVertex) + 0].position = topLeft;
			rVA[static_cast<size_t>(currentVertex) + 1].position = topRight;
			rVA[static_cast<size_t>(currentVertex) + 2].position = bottomRight;

			// Second triangle
			rVA[static_cast<size_t>(currentVertex) + 3].position = topLeft;
			rVA[static_cast<size_t>(currentVertex) + 4].position = bottomRight;
			rVA[static_cast<size_t>(currentVertex) + 5].position = bottomLeft;

			
			// Define position for right texture
			// Either boundary or inside
			if (h == 0 || h == worldHeight - 1 || w == 0 || w == worldWidth - 1)
			{

				// Triangle start
				rVA[static_cast<size_t>(currentVertex) + 0].texCoords = sf::Vector2f(0, 0 + TILE_TYPES * TILE_SIZE); // 0,150
				rVA[static_cast<size_t>(currentVertex) + 1].texCoords = sf::Vector2f(TILE_SIZE, 0 + TILE_TYPES * TILE_SIZE); // 50, 150
				rVA[static_cast<size_t>(currentVertex) + 2].texCoords = sf::Vector2f(TILE_SIZE, TILE_SIZE + TILE_TYPES * TILE_SIZE); // 50, 200
				
				
				// Repeating the triangle
				rVA[static_cast<size_t>(currentVertex) + 3].texCoords = sf::Vector2f(0, 0 + TILE_TYPES * TILE_SIZE); // 0, 150
				rVA[static_cast<size_t>(currentVertex) + 4].texCoords = sf::Vector2f(TILE_SIZE, TILE_SIZE + TILE_TYPES * TILE_SIZE); // 50, 200
				rVA[static_cast<size_t>(currentVertex) + 5].texCoords = sf::Vector2f(0, TILE_SIZE + TILE_TYPES * TILE_SIZE); // 0,200
			}
			else
			{
				srand((int)time(0) + (h * w) - h);
				int mOrG = (rand() % TILE_TYPES);
				int verticalOffset = mOrG * TILE_SIZE;

				rVA[static_cast<size_t>(currentVertex) + 0].texCoords = sf::Vector2f(0, 0 + verticalOffset);
				rVA[static_cast<size_t>(currentVertex) + 1].texCoords = sf::Vector2f(TILE_SIZE, 0 + verticalOffset);
				rVA[static_cast<size_t>(currentVertex) + 2].texCoords = sf::Vector2f(TILE_SIZE, TILE_SIZE + verticalOffset);


				rVA[static_cast<size_t>(currentVertex) + 3].texCoords = sf::Vector2f(0, 0 + verticalOffset);
				rVA[static_cast<size_t>(currentVertex) + 4].texCoords = sf::Vector2f(TILE_SIZE, TILE_SIZE + verticalOffset);
				rVA[static_cast<size_t>(currentVertex) + 5].texCoords = sf::Vector2f(0, TILE_SIZE + verticalOffset);

			}

			// Incrementing current vertex by number of vertices ie 6
			currentVertex += VERTS_IN_QUAD;
		}
	}

	return TILE_SIZE;

}