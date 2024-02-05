#include "Scene_Play.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "components.h"
#include "Action.h"

#include <iostream>
#include <memory>
#include <sstream>

Scene_Play::Scene_Play(GameEngine* gameEngine, const std::string& levelPath)
    : Scene(gameEngine), m_levelPath(levelPath)
{
    
    init(m_levelPath);
}

void Scene_Play::init(const std::string& levelPath)
{
    registerAction(sf::Keyboard::P,      "PAUSE");
    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::T,      "TOGGLE_TEXTURE");     // Toggle drawing (T)extures
    registerAction(sf::Keyboard::C,      "TOGGLE_COLLISION");   // Toggle drawing (C)ollision Boxes
    registerAction(sf::Keyboard::G,      "TOGGLE_GRID");        // Toggle drawing (G)rid
    registerAction(sf::Keyboard::P,      "PAUSE");              // Toggle pause (P)
    registerAction(sf::Keyboard::O,      "DEBUG");              // Toggle debug text (O)

    registerAction(sf::Keyboard::W,      "UP");
    registerAction(sf::Keyboard::A,      "LEFT");
    registerAction(sf::Keyboard::D,      "RIGHT");
    registerAction(sf::Keyboard::S,      "DOWN");
    registerAction(sf::Keyboard::Space,  "SHOOT");

    registerAction(sf::Keyboard::BackSpace, "SAVE");
    
    m_gridText.setCharacterSize(12);
    m_gridText.setFont(m_game->assets().getFont("Hack"));
    m_debugText.setFont(m_game->assets().getFont("Hack"));

    m_camera.init(m_game->window().getView(), &m_game->window(), { width() / 2, height() / 2 });

    m_xScroll = width() / 2;

    loadLevel(levelPath);
}

// If pos is inside the entity
bool IsInside(Vec2 pos, std::shared_ptr<Entity> e)
{
    auto ePos = e->getComponent<CTransform>().pos;
    auto size = e->getComponent<CAnimation>().animation.getSize();
    float dx = fabs(pos.x - ePos.x);
    float dy = fabs(pos.y - ePos.y);

    return (dx <= size.x / 2) && (dy <= size.y / 2);
}

void Scene_Play::loadLevel(const std::string& filename)
{
    sf::Clock clock;
    // rest the entity manager every time we load a level
    m_entityManager = EntityManager();

    std::ifstream f(filename);
    if (!f.is_open())
    {
        std::cerr << "Unable to open level file.\n";
        exit(1);
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

void Scene_Play::saveLevel(const std::string& filename)
{
    sf::Clock clock;

    std::ofstream f(filename);
    if (!f.is_open())
    {
        std::cerr << "Unable to open file.\n";
        exit(1);
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
            if (e->getComponent<CTransform>().velocity.y > m_playerConfig.MAXSPEED * 3)
            {
                // Terminal velocity
                e->getComponent<CTransform>().velocity.y = m_playerConfig.MAXSPEED * 3;
            }
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
        else if (action.name() == "RIGHT")
        {
            m_xScroll += m_scrollStep;
            std::cout << "m_xScroll: " << m_xScroll << "\n";
        }
        else if (action.name() == "LEFT")
        {
            m_xScroll -= m_scrollStep;
        }
        // mouse actions
        else if (action.name() == "LEFT_CLICK")
        {
            Vec2 worldPos = windowToWorld(action.pos());

            for (auto e : m_entityManager.getEntities())
            {
                if (e->hasComponent<CDraggable>() && IsInside(worldPos, e))
                {
                    std::cout << e->getComponent<CAnimation>().animation.getName() << std::endl;
                    e->getComponent<CDraggable>().dragging = !e->getComponent<CDraggable>().dragging;
                }
            }
        }
        else if (action.name() == "RIGHT_CLICK")
        {

        }
        else if (action.name() == "MIDDLE_CLICK")
        {

        }
        else if (action.name() == "MOUSE_MOVE")
        {
            m_mPos = action.pos();
            Vec2 worldPos = windowToWorld(m_mPos);
        }   
    }

    if (action.type() == "END")
    {
        if (action.name() == "LEFT")
        {
          
        }
        else if (action.name() == "RIGHT")
        {
         
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
    
    Vec2 camPos(m_xScroll, height() / 2);

    m_camera.setPos(camPos);
    m_camera.update();
    
    // draw all Entity textures / animations
    if (m_drawTextures)
    {
        for (auto e : m_entityManager.getEntities())
        {
            auto& transform = e->getComponent<CTransform>();

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
                std::string yCell = std::to_string((int)y / (int)m_gridSize.y);
                m_gridText.setString("(" + xCell + "," + yCell + ")");
                m_gridText.setPosition(x + 3, y + 2);
                m_game->window().draw(m_gridText);
            }
        }
    }

    if (m_losFlag)
    {
        // Draw LOS debug information
    }

    m_game->window().display();
    m_currentFrame++;
}


void Scene_Play::sDebug()
{
}

void Scene_Play::sDrag()
{
    for (auto e : m_entityManager.getEntities())
    {
        if (e->hasComponent<CDraggable>() && e->getComponent<CDraggable>().dragging)
        {
            Vec2 worldPos = windowToWorld(m_mPos);
            Vec2 gridPos = pixelToMidGrid(worldPos.x, worldPos.y, e);
            e->getComponent<CTransform>().pos = gridToMidPixel(gridPos.x, gridPos.y, e);
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

float Scene_Play::width() const
{
    return m_game->window().getSize().x;
}

float Scene_Play::height() const
{
    return m_game->window().getSize().y;
}
