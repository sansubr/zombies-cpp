#include "TextureHolder.h"

// Include the assert feature
#include <assert.h>

TextureHolder* TextureHolder::m_s_Instance = nullptr;
TextureHolder::TextureHolder()
{
	assert(m_s_Instance == nullptr);
	m_s_Instance = this;
}

sf::Texture& TextureHolder::GetTexture(std::string const& filename)
{
	// Get a reference to m_Textures using m_s_Instance
	auto& m = m_s_Instance->m_Textures;

	// Auto is the eqvuivalent of map::iterator
	// Create an iterator to hold a key-value pair
	// And search for the required kvp using the passed in filename
	auto keyValuePair = m.find(filename);

	// Did we find a match?
	if (keyValuePair != m.end())
	{
		// Yes
		// Return the texture
		// the second part of kvp, the texture
		return keyValuePair->second;
	}
	else
	{
		// Filename not found
		// Create a new keyvalue pair using the filename
		auto& texture = m[filename];

		// Load the texture from file in the usual way
		texture.loadFromFile(filename);

		// return the texture to the calling code;
		return texture;
	}

}