#include "tileMenu.h"

void TileMenu::loadTiles()
{
	// TODO: This loads all animations, including non tiles.
	//	     so this will need to be filtered later when deciding
	//		 which tiles to draw on the menu.

	for (auto const& [key, animation] : m_game->assets().getAnimationMap())
	{
		m_animations.push_back(std::make_shared<Animation>(animation));
	}
}

TileMenu::TileMenu(GameEngine* g)
	:m_game(g)
{
	loadTiles();
}
