#include "GameEngine.h"

void GameEngine::init(const std::string& path)
{

	m_assets.loadFromFile(path);

	m_window.create(sf::VideoMode(1920, 1080), "Level Editor");
	m_window.setFramerateLimit(60);
	m_window.setVerticalSyncEnabled(true);
	
	// calling false as not wanting default fonts
	ImGui::SFML::Init(m_window, false);
	// Get the ImGui IO
	auto &IO = ImGui::GetIO();
	IO.Fonts->Clear();
	// Load font for ImGui
	IO.Fonts->AddFontFromFileTTF("assets/Mario.ttf", 20.f);
	ImGui::SFML::UpdateFontTexture();
	//ImGui::PushFont(IO.Fonts->Fonts[0]);
	IO.FontGlobalScale = 1.0f;
	ImGui::StyleColorsClassic();

	changeScene("play", std::make_shared<Scene_Play>(this, "level1.txt"), true);
}

void GameEngine::update()
{

}

void GameEngine::userInput()
{
	sf::Event event;
	
	while (m_window.pollEvent(event))
	{
		ImGui::SFML::ProcessEvent(m_window, event);

		if (event.type == sf::Event::Closed)
		{
			quit();
		}

		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::X)
			{	
				// TODO: Use this to create a DOOM style melt effect
				//       when the game engine changes scenes
				sf::Texture texture;
				texture.create(m_window.getSize().x, m_window.getSize().y);
				texture.update(m_window);
				if (texture.copyToImage().saveToFile("test.png"))
				{
					std::cout << "screenshot saves to " << "test.png" << std::endl;
				}
			}
		}
		if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
		{
			// if the current scene does not have an action associated with this key, skip the event
			if (currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end())
			{
				continue;
			}

			//detrmine  start or end action by whether it was press or release
			const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";

			currentScene()->doAction(Action(currentScene()->getActionMap().at(event.key.code), actionType));
		}

		auto mousePos = sf::Mouse::getPosition(m_window);
		Vec2 mpos((float)mousePos.x, (float)mousePos.y);

		if (event.type == sf::Event::MouseButtonPressed)
		{
			switch (event.mouseButton.button)
			{
				case sf::Mouse::Left:   {currentScene()->doAction(Action("LEFT_CLICK",   "START", mpos)); break; }
				case sf::Mouse::Middle: {currentScene()->doAction(Action("MIDDLE_CLICK", "START", mpos)); break; }
				case sf::Mouse::Right:  {currentScene()->doAction(Action("RIGHT_CLICK",  "START", mpos)); break; }
				default: break;
			}
		}

		if (event.type == sf::Event::MouseButtonReleased)
		{
			switch (event.mouseButton.button)
			{
				case sf::Mouse::Left:   {currentScene()->doAction(Action("LEFT_CLICK",   "END", mpos)); break; }
				case sf::Mouse::Middle: {currentScene()->doAction(Action("MIDDLE_CLICK", "END", mpos)); break; }
				case sf::Mouse::Right:  {currentScene()->doAction(Action("RIGHT_CLICK",  "END", mpos)); break; }
				default: break;
			}
		}

		if (event.type == sf::Event::MouseMoved)
		{
			currentScene()->doAction(Action("MOUSE_MOVE", "START", Vec2((float)event.mouseMove.x, (float)event.mouseMove.y)));
		}
	}
}

std::shared_ptr<Scene> GameEngine::currentScene()
{
	return m_sceneMap[m_currentScene];
}

GameEngine::GameEngine(const std::string& path)
{
	init(path);
}

void GameEngine::changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene)
{
	m_currentScene = sceneName;
	m_sceneMap[sceneName] = scene;
}

void GameEngine::quit()
{
	m_running = false;
	ImGui::SFML::Shutdown();
	m_window.close();
}

void GameEngine::run()
{
	while (isRunning())
	{
		m_sceneMap[m_currentScene]->update();
		userInput();	
	}
}

sf::RenderWindow& GameEngine::window()
{
	return m_window;
}

Assets& GameEngine::assets()
{
	return m_assets;
}

bool GameEngine::isRunning()
{
	return m_running && m_window.isOpen();
}
