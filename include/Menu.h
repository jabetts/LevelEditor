#pragma once
#include <string>
#include <functional>
#include "imgui.h"
#include "EntityManager.h"
#include "Animation.h"

class GameEngine;
class Scene_Play;

class Menu
{
	GameEngine*					m_game      = nullptr;
	EntityManager*				m_entityManager = nullptr;
	Scene_Play*					m_scene = nullptr;
	bool						m_displaySaveWindow = false;
	bool						m_showWindow = false;
	sf::Clock					m_deltaClock;
	bool						m_edits = false; // This flag sets if we need to show save as on exit

private:
	Vec2 pixelToMidGrid(float x, float y, std::shared_ptr<Entity> entity);
	bool BeginCentered(const char* name);
	void drawSaveAsWindow();
	void drawMenuBar();
	void drawTabs();
	void drawFileMenu();
	void drawEditMenu();
	
public:
	Menu() {}
	Menu(GameEngine *game, EntityManager *entityManager)
	: m_game(game), m_entityManager(entityManager) {}
	void renderMenu(sf::View view);
	//void setFont(sf::Font font);
	void setEntityManager(EntityManager* entityManager);
	void setGameEngine(GameEngine* game);
	void setScene(Scene_Play* scene);
	void DrawStatusPopup();
	void drawEntityManagerMenu();
	void onUpdate();
	void drawMainMenu(struct Flags& flags);
};

