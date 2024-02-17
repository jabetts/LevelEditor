#pragma once

#include "Assets.h"
#include "EntityManager.h"
#include "GameEngine.h"

typedef std::vector<std::shared_ptr<Animation>> AnimationVec;

class TileMenu
{
	GameEngine*							m_game;
	AnimationVec						m_animations;
	sf::RectangleShape                  m_menuBorder;
	Vec2								m_menuPos;

	void loadTiles();
	void renderTileMenu();


public:
	TileMenu(GameEngine* g);
	void setBorderSize(Vec2 size);
};

