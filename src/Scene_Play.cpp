#include "Scene_Play.h"
#include "Physics.h"
#include "Assets.h"

#include "components.h"
#include "Action.h"

#include <iostream>
#include <memory>
#include <sstream>

/*
 * TODOS: - Seperate view(window) for assets able to be placed in the game world
 *          these should be clicked and dragged into the game world and click again
 *          to be placed on the level
 *        - Compression for the level files, this means they will not be able 
 *          to be editied manually by a text editor. Huffman compression would
 *          be OK
 *        - Undo system
 *        - Copy and Paste and delete (Ctrl-c, Ctrl-v, del) - tile select system already in place
 *        - Spawn a player to play the level
 *        - Update the render system. Have a render class that renders eahc view element seperately
 *        - This may require passing entitiyMaps - Graphics etc, rendering in specific orders i.e.
 *           - renderer.renderTiles
 *           - renderer.renderEffects
 *           - renderer.renderPlayer
 *           - renderer.renderUI
 *           - renderer.renderMenu
 *          This will require some planning and thought how I want to acheive this, but can then
 *          be used in other games
 *        - Menu, can base this on the menu system used in the DOOM source code for learning
 */

Scene_Play::Scene_Play(GameEngine* gameEngine, const std::string& levelPath)
    : Scene(gameEngine), m_levelPath(levelPath), m_tileMenu(&m_game->assets())
{
    init(m_levelPath);
}

void actionMenu(int n)
{
    switch (n)
    {

    }
}

void Scene_Play::init(const std::string& levelPath)
{
    // Options
    registerAction(sf::Keyboard::P,         "PAUSE");
    registerAction(sf::Keyboard::Escape,    "QUIT");
    registerAction(sf::Keyboard::T,         "TOGGLE_TEXTURE");     // Toggle drawing (T)extures
    registerAction(sf::Keyboard::C,         "KEYC");               // C key, for collisions and Ctrl-c
    registerAction(sf::Keyboard::V,         "KEYV");
    registerAction(sf::Keyboard::G,         "TOGGLE_GRID");        // Toggle drawing (G)rid
    registerAction(sf::Keyboard::O,         "DEBUG");              // Toggle debug text (O)
    registerAction(sf::Keyboard::BackSpace, "SAVE");
    registerAction(sf::Keyboard::L,         "LOAD");
    registerAction(sf::Keyboard::LControl,  "LCTRL");
    registerAction(sf::Keyboard::RControl,  "RCTRL");

    // Actions
    registerAction(sf::Keyboard::F,         "FLIP");
    registerAction(sf::Keyboard::Delete,    "DELETE");
    // Movement
    registerAction(sf::Keyboard::W,         "UP");
    registerAction(sf::Keyboard::A,         "LEFT");
    registerAction(sf::Keyboard::D,         "RIGHT");
    registerAction(sf::Keyboard::S,         "DOWN");
    registerAction(sf::Keyboard::F2,        "TILE_MENU");

    // Text for the grid
    m_gridText.setCharacterSize(12);
    m_gridText.setFont(m_game->assets().getFont("Hack"));
    m_debugText.setFont(m_game->assets().getFont("Hack"));

    // Initialise starting point of the main view
    m_xScroll = width() / 2; 

    // Initialise Menu system
    Menu::MenuItem mainMenu[] = {
        {1, "M_NLEVEL",'n', actionMenu},
        {1, "M_SAVE",  's', actionMenu},
        {1, "M_LOAD",  'l', actionMenu},
        {1, "M_QUIT",  'q', actionMenu},
    };

    loadLevel(levelPath);
}

// Check if a point is inside an entity
bool IsInside(Vec2 pos, std::shared_ptr<Entity> e)
{
    auto ePos = e->getComponent<CTransform>().pos;
    auto size = e->getComponent<CAnimation>().animation.getSize();
    float dx = fabs(pos.x - ePos.x);
    float dy = fabs(pos.y - ePos.y);

    return (dx <= size.x / 2) && (dy <= size.y / 2);
}

bool isInsideBox(Vec2 pos, Vec2 target, size_t size)
{
    if (pos.x < target.x - (size / 2) || pos.x > target.x + (size / 2))
    {
        return false;
    }
    if (pos.y < (target.y - size / 2) || pos.y > target.y + (size / 2))
    {
        return false;
    }
    return true;
}

void Scene_Play::loadLevel(const std::string& filename)
{
    sf::Clock clock;
    // rest the entity manager every time we load a level
    m_entityManager = EntityManager();

    std::ifstream f(filename);
    if (!f.is_open())
    {
        // No level file given
        return;
    }

    std::cout << "Loading level\n";
    std::string line;
    
    while (std::getline(f, line))
    {
        // Skip comments
        if (line[0] == '#')
        {
            continue;
        }

        std::istringstream iss(line);
        std::string in;
        iss >> in;

        if (in == "Tile")
        {
            std::string text;
            float X, Y;
            iss >> text >> X >> Y;
            auto t = m_entityManager.addEntity("Tile");
            t->addComponent<CAnimation>(m_game->assets().getAnimation(text), true);
            t->addComponent<CBoundingBox>(m_game->assets().getAnimation("Ground").getSize());
            t->addComponent<CTransform>(gridToMidPixel(X, Y, t));
            t->addComponent<CDraggable>();
        }
        else if (in == "Dec")
        {
            std::string text;
            float X, Y;
            iss >> text >> X >> Y;
            auto t = m_entityManager.addEntity("Dec");
            t->addComponent<CAnimation>(m_game->assets().getAnimation(text), true);
            t->addComponent<CTransform>(gridToMidPixel(X, Y, t));

        }
        // TODO: for the level editor, this will just be for for players starting position
        //       their gravity etc.
        else if (in == "Player")
        {
            iss >> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.CW
                >> m_playerConfig.CH >> m_playerConfig.SPEED >> m_playerConfig.JUMP
                >> m_playerConfig.MAXSPEED >> m_playerConfig.GRAVITY >> m_playerConfig.WEAPON;
        }
    }
    if (f.is_open()) { f.close(); }
    int time = clock.getElapsedTime().asMilliseconds();
    std::cout << "Level loaded in: " << time << " milliseconds\n";
}

void Scene_Play::loadLevel(int i)
{
    return;
}

void Scene_Play::saveLevel(const std::string& filename)
{
    sf::Clock clock;

    std::ofstream f(filename);
    // No level file
    if (!f.is_open())
    {
        std::cerr << "Unable to save to file.\n";
        return;
    }

    std::string line;

    f << "##### Tile data #####" << std::endl;

    for (auto& e : m_entityManager.getEntities("Tile"))
    {
        Vec2 gridPos = pixelToMidGrid(e->getComponent<CTransform>().pos.x, e->getComponent<CTransform>().pos.y, e);
        f << "Tile" << " " << e->getComponent<CAnimation>().animation.getName() << " "
            << gridPos.x << " " << gridPos.y << std::endl;
    }

    f << "##### Decoration data #####" << std::endl;

    for (auto& e : m_entityManager.getEntities("Dec"))
    {
        Vec2 gridPos = pixelToMidGrid(e->getComponent<CTransform>().pos.x, e->getComponent<CTransform>().pos.y, e);
        f << "Dec" << " " << e->getComponent<CAnimation>().animation.getName() << " "
            << gridPos.x << " " << gridPos.y + 1 << std::endl;
    }

    for (auto& e : m_entityManager.getEntities("Player"))
    {
        Vec2 gridPos = pixelToMidGrid(e->getComponent<CTransform>().pos.x, e->getComponent<CTransform>().pos.y, e);
        f << "Dec" << " " << e->getComponent<CAnimation>().animation.getName() << " "
            << gridPos.x << " " << gridPos.y << std::endl;
    }

    f.close();
    std::cout << "file saved\n";
}

Vec2 Scene_Play::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{ 
    auto size = entity->getComponent<CAnimation>().animation.getSprite().getOrigin();
    float pixelX = (gridX * (float)m_gridSize.x) + size.x;
    float pixelY = height() - (gridY * m_gridSize.y) - size.y;

    return Vec2(pixelX, pixelY);
}

// Returns the grid co-ordinate of a sprite origin.
Vec2 Scene_Play::pixelToMidGrid(float x, float y, std::shared_ptr<Entity> entity)
{
    auto size = entity->getComponent<CAnimation>().animation.getSprite().getOrigin();
    y = height() - y;
    float gridX = x / m_gridSize.x;
    float gridY = y / m_gridSize.y;

    return Vec2(std::floor(gridX), std::floor(gridY));
}

Vec2 Scene_Play::windowToWorld(const Vec2& window) const
{
    auto& view = m_game->window().getView();

    float wx = view.getCenter().x - m_game->window().getSize().x / 2;
    float wy = view.getCenter().y - m_game->window().getSize().y / 2;

    return Vec2(window.x + wx, window.y + wy);
}

void Scene_Play::spawnPlayer()
{

}

void Scene_Play::spawnBullet(std::shared_ptr<Entity> entity)
{

}

void Scene_Play::update()
{
    m_entityManager.update();
    if (!m_paused)
    {
        sMovement();
        sLifeSpan();
        sCollision();
        sAnimation();
        sDrag();
    }
    m_currentFrame++;
    sRender();
}

void Scene_Play::sMovement()
{
    for (auto e : m_entityManager.getEntities())
    {
        // Do gravity first
        if (e->hasComponent<CGravity>())
        {
            e->getComponent<CTransform>().velocity.y += e->getComponent<CGravity>().gravity;
        }

        // Add the velocities to the entities
        e->getComponent<CTransform>().pos += e->getComponent<CTransform>().velocity;

        // Flip sprite based on movement direction
        if (e->getComponent<CTransform>().velocity.x < 0)
        {
            e->getComponent<CTransform>().scale.x = -1.0f;
        }
        if (e->getComponent<CTransform>().velocity.x > 0)
        {
            e->getComponent<CTransform>().scale.x = 1.0f;
        }
    }
}

void Scene_Play::sLifeSpan()
{

}

void Scene_Play::sCollision()
{

}

void Scene_Play::sDoAction(const Action& action)
{
    if (action.type() == "START")
    {
        // non game play actions
        if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
        else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
        else if (action.name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; std::cout << "Grid " << (m_drawGrid ? "On\n" : "Off\n"); }
        else if (action.name() == "PAUSE") { setPaused(!m_paused); }
        else if (action.name() == "QUIT") { onEnd(); }
        else if (action.name() == "COLLISIONS") { m_collisions = !m_collisions; }
        else if (action.name() == "DEBUG") { m_debugFlag = !m_debugFlag; }
        else if (action.name() == "SAVE") { saveLevel("level2.txt"); }
        else if (action.name() == "TILE_MENU") { m_displayTileMenu = !m_displayTileMenu; }
        
        // movement keys - basic movement and mouse acceleration if mouse with 100 pixels of each side of
        // the window. TODO: make the mouse position a percantage rather than hard coded pixels
        else if (action.name() == "RIGHT")
        {
            std::cout << m_scrollStep << "\n";
            if (m_mousePos.winPos.x > width() - 100)
            {
                m_xScroll += m_scrollStep + m_mouseScrollAcc++;
            }
            else
            {
                m_xScroll += m_scrollStep;
                m_mouseScrollAcc = 20;
            }
        }
        else if (action.name() == "LEFT")
        {
            std::cout << m_scrollStep << "\n";
            if (m_mousePos.winPos.x < 100)
            {
                if (m_xScroll > width() / 2)
                    m_xScroll -= m_scrollStep + m_mouseScrollAcc++;;
            }
            else
            {
                if(m_xScroll > width() / 2)
                    m_xScroll -= m_scrollStep;
                    m_mouseScrollAcc = 20;
            }
        }
        // TODO: This flip is not saved to the file as the current save file format does not include a scale.x
        //       will need to update the level file spec to support this.
        else if (action.name() == "FLIP")
        {
            if (m_selectedEntity != nullptr)
            {
                m_selectedEntity->getComponent<CTransform>().scale.x = -m_selectedEntity->getComponent<CTransform>().scale.x;
            }
        }
        // mouse actions
        else if (action.name() == "LEFT_CLICK")
        {
            for (auto e : m_entityManager.getEntities())
            {
                if (e->hasComponent<CDraggable>() && IsInside({ m_mousePos.worldPos.x, m_mousePos.worldPos.y }, e))
                {
                    std::cout << e->getComponent<CAnimation>().animation.getName() << std::endl;
                    e->getComponent<CDraggable>().dragging = !e->getComponent<CDraggable>().dragging;
                }
            }

            if (m_displayTileMenu)
            {
                for (auto& a : m_tileMenu.animations())
                {
                    if (isInsideBox({ (float)m_mousePos.winPos.x, (float)m_mousePos.winPos.y }, { a->getSprite().getPosition().x,
                        a->getSprite().getPosition().y }, 64))
                    {
                        printf("Animation selected: %s: x: %f y: %f size: %f\n", a->getName().c_str(), 
                            a->getSprite().getPosition().x, a->getSprite().getPosition().y,
                            a->getSize().x);

                        // Create the tile entity, make it draggable and set it to dragging
                        auto t = m_entityManager.addEntity("Tile");
                        t->addComponent<CAnimation>(m_game->assets().getAnimation(a->getName().c_str()), true);
                        t->addComponent<CBoundingBox>(m_game->assets().getAnimation("Ground").getSize());
                        t->addComponent<CTransform>(gridToMidPixel(m_mousePos.gridPos.x, m_mousePos.gridPos.y, t));
                        t->addComponent<CDraggable>();
                        t->getComponent<CDraggable>().dragging = true;
                    }
                }
            }
       
        }
        else if (action.name() == "RIGHT_CLICK")
        {

            if (m_selectedEntity != nullptr)
            {
                m_selectedEntity = nullptr;
            }

            for (auto e : m_entityManager.getEntities())
            {
                if (e->hasComponent<CDraggable>() && IsInside({ m_mousePos.worldPos.x, m_mousePos.worldPos.y }, e))
                {
                        m_selectedEntity = e;
                }
            }
        }
        else if (action.name() == "MIDDLE_CLICK")
        {

        }
        if (action.name() == "MOUSE_MOVE")
        {
            //m_mPos = action.pos();
            //Vec2 worldPos = windowToWorld(m_mPos);

            updateMouseCoords(action.pos());
            
            m_debugText.setCharacterSize(20);
            m_debugText.setFillColor(sf::Color::White);
            m_debugText.setOutlineColor(sf::Color::Black);
            m_debugText.setFont(m_game->assets().getFont("Mario"));
            std::stringstream ss;

            ss << "Window pos: " << m_mousePos.winPos.x << " " << m_mousePos.winPos.y << "\n"
                << "World pos: " << m_mousePos.worldPos.x << " " << m_mousePos.worldPos.y << "\n"
                << "Grid pos: " << m_mousePos.gridPos.x << " " << m_mousePos.gridPos.y << "\n";

            m_debugText.setString(ss.str());

        }

        else if (action.name() == "LCTRL" || action.name() == "RCTRL")
        {
            if (action.name() == "CKEY")
            {
                // Copy entity
            }
            if (action.name() == "VKEY")
            {
                // paste entity
            }
        }
    }

    if (action.type() == "END")
    {
        if (action.name() == "LEFT")
        {
            m_scrollStep = 20;
        }
        else if (action.name() == "RIGHT")
        {
            m_scrollStep = 20;
        }
    }
}

void Scene_Play::sAnimation()
{
    for (auto& e : m_entityManager.getEntities())
    {
        if (e->hasComponent<CAnimation>())
        {
            e->getComponent<CAnimation>().animation.update();
            if (!e->getComponent<CAnimation>().repeat)
            {
                if (e->getComponent<CAnimation>().animation.hasEnded())
                {
                    e->destroy();
                }
            }
        }
    }
}

void Scene_Play::onEnd()
{
    m_game->quit();
}

void Scene_Play::sRender()
{
    m_game->window().clear(sf::Color(148, 148, 255));
    
    m_view = m_game->window().getView();
    sf::View map = m_game->window().getView();
    sf::View ui = m_game->window().getDefaultView();

    float viewX = std::max((float)m_xScroll, width() / 2.0f);
    m_view.setCenter(viewX, height() / 2);

    map.setCenter(m_view.getCenter());
    map.setViewport(sf::FloatRect(0.75f, 0.f, 0.25f, 0.25f));

    // Render tile elements
    m_game->window().setView(m_view);

    sf::RectangleShape selectRect;
    
    // draw all Entity textures / animations
    if (m_drawTextures)
    {
        for (auto e : m_entityManager.getEntities())
        {
            auto& transform = e->getComponent<CTransform>();

            if (IsInside({m_mousePos.worldPos.x, m_mousePos.worldPos.y}, e))
            {
                // Draw a red outline around the entity
                auto& box = e->getComponent<CBoundingBox>();
                auto& transform = e->getComponent<CTransform>();
            
                selectRect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
                selectRect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
                selectRect.setPosition(transform.pos.x, transform.pos.y);
                selectRect.setFillColor(sf::Color(0, 0, 0, 0));
                selectRect.setOutlineColor(sf::Color::Red);
                selectRect.setOutlineThickness(3);
            }

            if (e->hasComponent<CAnimation>())
            {
                auto& animation = e->getComponent<CAnimation>().animation;
                animation.getSprite().setRotation(transform.angle);
                animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
                animation.getSprite().setScale(transform.scale.x, transform.scale.y);
                m_game->window().draw(animation.getSprite());

            }
        }
    }

    // draw all Entity collision bounding boxes with a rectangle shape
    if (m_drawCollision)
    {
        for (auto e : m_entityManager.getEntities())
        {
            if (e->hasComponent<CBoundingBox>())
            {
                auto& box = e->getComponent<CBoundingBox>();
                auto& transform = e->getComponent<CTransform>();
                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
                rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
                rect.setPosition(transform.pos.x, transform.pos.y);
                rect.setFillColor(sf::Color(0, 0, 0, 0));
                rect.setOutlineColor(sf::Color(255, 255, 255, 255));
                rect.setOutlineThickness(1);
                m_game->window().draw(rect);
            }
        }
    }

    // draw the grid so that students can easily debug
    if (m_drawGrid)
    {
        float leftX = m_game->window().getView().getCenter().x - width() / 2;
        float rightX = leftX + width() + m_gridSize.x;
        float nextGridX = leftX - ((int)leftX % (int)m_gridSize.x);

        for (float x = nextGridX; x < rightX; x += m_gridSize.x)
        {
            drawLine(sf::Vector2f(x, 0), sf::Vector2f(x, height()));

            for (float y = 0; y < height(); y += m_gridSize.y)
            {
                if (m_debugFlag)
                {
                    drawLine(sf::Vector2f(x, y), sf::Vector2f(x + m_gridSize.x, y + m_gridSize.y));
                    drawLine(sf::Vector2f(x + m_gridSize.x, y), sf::Vector2f(x, y + m_gridSize.y));
                }
                drawLine(sf::Vector2f(leftX, height() - y), sf::Vector2f(rightX, height() - y));
                std::string xCell = std::to_string((int)x / (int)m_gridSize.x);
                std::string yCell = std::to_string((int)(height() - y) / (int)m_gridSize.y);
                m_gridText.setString("(" + xCell + "," + yCell + ")");
                m_gridText.setPosition(x + 3, y + 2);
                m_game->window().draw(m_gridText);
                // TODO: Draw a box around the mouse grid position
            }
        }
    }

    // Draw box around selected (right clicked) entity
    // TODO: Move this stuff to a function that just returns the rectange
    sf::RectangleShape selected;
    if (m_selectedEntity != nullptr)
    {
        auto& box = m_selectedEntity->getComponent<CBoundingBox>();
        auto& transform = m_selectedEntity->getComponent<CTransform>();
        selected.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
        selected.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
        selected.setPosition(transform.pos.x, transform.pos.y);
        selected.setFillColor(sf::Color(0, 0, 0, 0));
        selected.setOutlineColor(sf::Color::Green);
        selected.setOutlineThickness(4);
        m_game->window().draw(selected);
    }

    // UI highlights - these need to be drawn in view after everything else.
    m_game->window().draw(selectRect);
    m_game->window().draw(selected);

    // Static UI elements
    m_game->window().setView(ui);
    m_game->window().draw(m_debugText);
    // draw tile menu
    if (m_displayTileMenu)
    {
        m_tileMenu.renderTileMenu(m_game->window());
    }

    m_game->window().display();
    m_currentFrame++;
}

void Scene_Play::sDebug()
{
}


// TODO: Fix this as y is reversed.
void Scene_Play::sDrag()
{
    for (auto e : m_entityManager.getEntities())
    {
        if (e->hasComponent<CDraggable>() && e->getComponent<CDraggable>().dragging)
        {
            Vec2 worldPos = windowToWorld(m_mPos);
            Vec2 gridPos = pixelToMidGrid(worldPos.x, worldPos.y, e);
            e->getComponent<CTransform>().pos = gridToMidPixel(m_mousePos.gridPos.x, m_mousePos.gridPos.y, e);
        }
    }
}

void Scene_Play::sEnemySpawner()
{
}

void Scene_Play::drawLine(sf::Vector2f v1, sf::Vector2f v2)
{
    sf::VertexArray line(sf::Lines, 2);
    line[0].position = v1;
    line[1].position = v2;
    line[0].color = sf::Color::White;
    line[1].color = sf::Color::White;

    m_game->window().draw(line);
}

void Scene_Play::updateMouseCoords(Vec2 mousePos)
{
    m_game->window().setView(m_view);
    m_mousePos.screenPos = sf::Mouse::getPosition();
    m_mousePos.winPos = {static_cast<int>(mousePos.x), static_cast<int>(mousePos.y)};
    m_mousePos.worldPos = m_game->window().mapPixelToCoords(m_mousePos.winPos);
    m_mousePos.gridPos.x = m_mousePos.worldPos.x / m_gridSize.x;
    m_mousePos.gridPos.y = static_cast<int>((height()) - m_mousePos.worldPos.y) / m_gridSize.y;
}

float Scene_Play::width() const
{
    return m_game->window().getSize().x;
}

float Scene_Play::height() const
{
    return m_game->window().getSize().y;
}
