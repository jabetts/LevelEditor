#pragma once
#include "GameEngine.h"
#include <SFML/Graphics.hpp>

class Renderer
{
	Renderer(GameEngine* game);
	sf::View world;
	sf::View map;
	sf::View ui;
	sf::View menu;

public:
	void renderTiles();
	void renderEffects();
	void renderUI();
	void renderPlayer();
};

