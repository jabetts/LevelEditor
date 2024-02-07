#pragma once

#include "Entity.h"
#include <string>

class Action
{
	// This is used to track where the mouse is in the different coord systems
	std::string m_name = "NONE";
	std::string m_type = "NONE";
	// This is the screen mouse pos - gets transformed by the scene_play class
	Vec2		m_pos = Vec2(0, 0);


public:

	Action();
	Action(const std::string & name, const std::string & type);
	Action(const std::string& name, const std::string& type, const Vec2& pos);

	const std::string & name() const;
	const std::string & type() const;
	const Vec2& pos() const;
	std::string toString() const;
};

