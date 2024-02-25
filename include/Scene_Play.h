#pragma once

#include <map>
#include <memory>
#include <algorithm>
#include "Scene.h"
#include "Menu.h"
#include "EntityManager.h"
#include "GameEngine.h"
#include "tileMenu.h"

//class GameEngine;

typedef sf::Vector2f Vec2f;
typedef sf::Vector2i Vec2i;
typedef sf::Vector2u Vec2u;


class Scene_Play : public Scene
{
	// This is used to configure the player start location, speed, gravity etc.
	// it is written to the level file and is loaded by hte level loader
	struct PlayerConfig
	{
		float X, Y, CW, CH, SPEED, MAXSPEED, JUMP, GRAVITY;
		std::string WEAPON;
	};

	struct MousePos
	{
		Vec2i						winPos;
		Vec2f						worldPos;
		Vec2u						gridPos;
		Vec2i						screenPos;
	};

protected:
	std::shared_ptr<Entity>			m_player;
	std::shared_ptr<Entity>			m_selectedEntity;
	std::string						m_levelPath;
	std::string						m_savePath;
	PlayerConfig					m_playerConfig;
	MousePos                        m_mousePos;
	Menu							m_menu;
	TileMenu                        m_tileMenu;
	bool							m_drawTextures = true;
	bool							m_drawCollision = false;
	bool							m_drawGrid = false;
	bool							m_collisions = true;
	bool							m_debugFlag = false;
	bool							m_losFlag = false;
	bool							m_gridSnap = true;
	bool							m_displayTileMenu = true;
	bool							m_displaySaveWindow = false;
	int								m_xScroll = 0;
	int								m_scrollStep = 20;
	int								m_mouseScrollAcc = 10;
	sf::Text						m_gridText;
	sf::Text						m_debugText;
	Vec2							m_gridSize = {64, 64};
	Vec2							m_mPos;
	Vec2							m_tileMenuPos;
	sf::CircleShape					m_shape;
	sf::View						m_view;
	sf::Clock						m_deltaClock;
	char                            filenameBuffer[255] = "out.txt";
	
	void init(const std::string& levelPath);
	void loadLevel(const std::string& filename);
	void loadLevel(int i);
	void saveLevel(const std::string& filename);
	void resetLevel();

	void sAnimation();
	void sMovement();
	void sEnemySpawner();
	void sCollision();
	void sLifeSpan();
	void sDebug();
	void sDrag();
	Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);
	Vec2 pixelToMidGrid(float x, float y, std::shared_ptr<Entity> entity);
	Vec2 windowToWorld(const Vec2& window) const;

	void spawnPlayer();
	void spawnBullet(std::shared_ptr<Entity> entity);
	void drawLine(sf::Vector2f v1, sf::Vector2f v2);
	void updateMouseCoords(Vec2 mousePos);
	void renderTileMenu(Vec2 pos);
	void sMenu();
	void appMainMenuBar();
	float width() const;
	float height() const;

public:
	Scene_Play(GameEngine* gameEngine, const std::string& levelPath);
	void update();
	void onEnd();
	void sDoAction(const Action& action);
	void sRender();
};

