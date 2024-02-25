#include "tileMenu.h"

void TileMenu::loadTiles()
{
	// TODO: This loads all animations, including non tiles.
	//	     so this will need to be filtered later when deciding
	//		 which tiles to draw on the menu.

	for (auto const& [key, animation] : m_assets->getAnimationMap())
	{
		m_animations.push_back(std::make_shared<Animation>(animation));
	}
}

void TileMenu::renderTileMenu(sf::RenderWindow &window)
{
	size_t x = 0;
	size_t y = 0;
	for (auto& a : m_animations)
	{
		if (a->getSize().x <= 64)
		{
			a->getSprite().setPosition(m_menuPos.x + x, m_menuPos.y + y);
				window.draw(a->getSprite());
				x += a->getSize().x;
		}
		
		if (x > (8 * 64))
		{
			y += 64;
			x = 0;
		}
	}
}

TileMenu::TileMenu(Assets* assets)
	:m_assets(assets)
{
	m_menuPos.x = 128;
	m_menuPos.y = 128;
	loadTiles();
}

AnimationVec& TileMenu::animations()
{
	return m_animations;
}

