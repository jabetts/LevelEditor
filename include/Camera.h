#pragma once

#include <SFML/Graphics.hpp>
#include "vec2.h"

class GameEngine;

class Camera
{
	GameEngine*			m_game = nullptr;
	Vec2				m_viewPos;
	Vec2                m_vel = { 0, 0 };
	sf::View			m_view;
	sf::RenderWindow*   m_window;

public:
	Camera();
	~Camera();
	sf::View& view();
	void init(sf::View view, sf::RenderWindow* window, Vec2 pos);
	Vec2 pos();
	void setPos(Vec2 pos);
	void update();
};

