#pragma once
#include <string>
#include <functional>

#include "Animation.h"

class GameEngine;

class Menu
{
	struct MenuItem;
	
	GameEngine*					m_game      = nullptr;
	Menu*						m_prevMenu  = nullptr;
	MenuItem*					m_menuItems = nullptr;
	int							m_numItems  = 0;
	int							m_iconXoff  = -64;  // icons x offset from menu item
	float						m_x, m_y;			// x, y of top left of menu in window coords
	short						m_lastOn    = 0;	// last menu item the user was on
	short						m_itemOn    = 0;    // item the icon is on

public:
	struct MenuItem
	{
		int status;
		std::string name;
		char alphaKey;
		//std::function<void(int)> routine; // TODO: use this isntead of raw function pointers
		void (*function)(int choice);
		MenuItem(int status, const std::string& name, char alphaKey,
			void(*function)(int choice))
			: status(status), name(name), alphaKey(alphaKey), function(function) {}
	};
	Menu() {}
	Menu(GameEngine *game, MenuItem* mItems, Menu* prevMenu, int iconXoff, int x, int y)
	: m_game(game), m_menuItems(mItems), m_prevMenu(prevMenu),m_iconXoff(iconXoff), m_x(x), m_y(y) 
	{
		// TODO: set number of menu items m_numItems
	}

	void setItems(MenuItem* menuItems);
	void renderMenu(sf::View view);
	void setIconOffset(int);
};

