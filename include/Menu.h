#pragma once
#include <string>
#include <functional>
#include "imgui.h"
#include "EntityManager.h"
#include "Animation.h"

class GameEngine;



class Menu
{
	GameEngine*					m_game      = nullptr;
	EntityManager*				m_entityManager = nullptr;

	Vec2 pixelToMidGrid(float x, float y, std::shared_ptr<Entity> entity);
	void DrawStatusPopup();
	bool BeginCentered(const char* name);

public:
	
	Menu() {}
	Menu(GameEngine *game, EntityManager *entityManager)
	: m_game(game), m_entityManager(entityManager)
	{
	}

	void renderMenu(sf::View view);
	void drawEntityManagerMenu();
	void setIconOffset(int iconOffset);
	void setFont(sf::Font font);
	void setEntityManager(EntityManager* entityManager);
	void setGameEngine(GameEngine* game);
};

