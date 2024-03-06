#include "Camera.h"

Camera::Camera()
{
}

void Camera::init(sf::View view, sf::RenderWindow* window, Vec2 pos)
{
	m_view = view;
	m_window = window;
	m_viewPos = pos;
}
Vec2 Camera::pos()
{
	return m_viewPos;
}
void Camera::setPos(Vec2 pos)
{
	m_viewPos = pos;
}
void Camera::update()
{
	float viewX = std::max(m_viewPos.x + m_vel.x, m_window->getSize().x / 2.0f);
	m_view.setCenter(viewX, (float)m_window->getSize().y / 2);
	m_window->setView(m_view);
}
Camera::~Camera()
{
}

sf::View& Camera::view()
{
	return m_view;
}






