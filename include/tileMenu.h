#pragma once

#include <SFML/Graphics.hpp>
#include "Assets.h"

typedef std::vector<std::shared_ptr<Animation>> AnimationVec;

class TileMenu
{
	AnimationVec						m_animations;
	sf::RectangleShape                  m_menuBorder;
	Vec2								m_menuPos;
	Assets*								m_assets;

	void loadTiles();
	void drawTile();

public:
	TileMenu(Assets* assets);
	void setBorderSize(Vec2 size);
	void renderTileMenu(sf::RenderWindow &window);
	AnimationVec& animations();
};

