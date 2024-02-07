#pragma once

#include <map>
#include <memory>
#include <algorithm>
#include <sstream>
#include "Camera.h"
#include "Scene.h"
#include "EntityManager.h"

class Scene_Play : public Scene
{
	// This is used to configure the player tart location, speed, gravity etc.
	// it is written to the level file and is loaded by hte level loader
	struct PlayerConfig
	{
		float X, Y, CW, CH, SPEED, MAXSPEED, JUMP, GRAVITY;
		std::string WEAPON;
	};

	struct MousePos
	{
		sf::Vector2i				winPos;
		sf::Vector2f				worldPos;
		sf::Vector2u				gridPos;
		sf::Vector2i				screenPos;
	};

protected:

	std::shared_ptr<Entity>			m_player;
	std::shared_ptr<Entity>			m_selectedEntity;
	std::string						m_levelPath;
	PlayerConfig					m_playerConfig;
	MousePos                        m_mousePos;
	bool							m_drawTextures = true;
	bool							m_drawCollision = false;
	bool							m_drawGrid = false;
	bool							m_collisions = true;
	bool							m_debugFlag = false;
	bool							m_losFlag = false;
	bool							m_gridSnap = true;
	int								m_xScroll = 0;
	int								m_scrollStep = 10;
	sf::Text						m_gridText;
	sf::Text						m_debugText;
	Vec2							m_gridSize = {64, 64};
	Vec2							m_mPos;
	sf::CircleShape					m_shape;
	sf::View						m_view;
	



	void init(const std::string& levelPath);
	void loadLevel(const std::string& filename);
	void loadLevel();
	void saveLevel(const std::string& filename);

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

	float width() const;
	float height() const;

public:
	Scene_Play(GameEngine* gameEngine, const std::string& levelPath);
	void update();
	void onEnd();
	void sDoAction(const Action& action);
	void sRender();
};

