#include "Menu.h"
#include "GameEngine.h"
#include "Scene_Play.h"

// Returns the grid co-ordinate of a sprite origin.
Vec2 Menu::pixelToMidGrid(float x, float y, std::shared_ptr<Entity> entity)
{
    auto size = entity->getComponent<CAnimation>().animation.getSprite().getOrigin();
    y = (float)m_game->window().getSize().y;
    float gridX = x / 64; // TODO: Need to change this so it uses the scene_Play version of the function
    float gridY = y / 64;

    return Vec2(std::floor(gridX), std::floor(gridY));
}

bool Menu::BeginCentered(const char* name) {
    ImGuiIO const& io = ImGui::GetIO();
    ImVec2 pos(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings;
    return ImGui::Begin(name, nullptr, flags);
}

void Menu::setEntityManager(EntityManager* entityManager)
{
    m_entityManager = entityManager;
}

void Menu::setGameEngine(GameEngine* game)
{
    m_game = game;
}

void Menu::drawMainMenu(struct Flags& flags)
{
    ImGui::Begin("Level editor", &m_showWindow, ImGuiWindowFlags_MenuBar);
    
    drawMenuBar();
    
    ImGui::Text("Display");
    ImGui::Checkbox("Grid", &flags.drawGrid);
    ImGui::SameLine();
    ImGui::Checkbox("Collisions", &flags.drawCollisions);
    ImGui::SameLine();
    ImGui::Checkbox("Textures", &flags.drawTextures);
    ImGui::Separator();


}

void Menu::drawFileMenu()
{
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("New"))
        {

        }
        if (ImGui::MenuItem("Open"))
        {

        }
        if (ImGui::BeginMenu("Open Recent"))
        {
            // TODO: Create open recent function which will
            //       load a text file with last 5 levels opened
            ImGui::MenuItem("Level1.txt");
            ImGui::EndMenu();
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Save"))
        {
            //saveLevel(filenameBuffer);
            //m_displaySaveWindow = false;
        }
        if (ImGui::MenuItem("Save as.."))
        {
            m_displaySaveWindow = true;
            DrawStatusPopup();
        }
        ImGui::Separator();

        if (ImGui::MenuItem("Quit"))
        {
            m_game->quit();
        }
        ImGui::EndMenu();
    }
}

void Menu::drawEditMenu()
{
}

void Menu::onUpdate()
{
    ImGui::NewFrame();
    //bool demoWindow = true;
    //ImGui::ShowDemoWindow(&demoWindow);

    ImGui::SFML::Update(m_game->window(), m_deltaClock.restart());
    ImGui::Render();
}

void Menu::drawMenuBar()
{
    // Menu bar items
    if (ImGui::BeginMenuBar())
    {
        drawFileMenu();
        drawEditMenu();
        ImGui::EndMenuBar();
    }
}

// TODO: Need to change the asset import spec so each texture has additional meta data
//       so we can just loop through certain textures and use those to load every tile
//       or sprite we want based on the type.
void Menu::drawTabs()
{
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

    if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("Tiles"))
        {
            const sf::Texture& brick = m_game->assets().getTexture("Brick");
            const sf::Texture& ground = m_game->assets().getTexture("Ground");
            const sf::Texture& block = m_game->assets().getTexture("Block");
            const sf::Texture& basalt = m_game->assets().getTexture("Basalt");
            const sf::Vector2f size(65.0f, 65.0f);

            if (ImGui::ImageButton("Brick", brick, size, sf::Color::Transparent, sf::Color(255, 255, 255)))
            {
                auto t = m_entityManager->addEntity("Tile");
                t->addComponent<CAnimation>(m_game->assets().getAnimation("Brick"), true);
                t->addComponent<CBoundingBox>(m_game->assets().getAnimation("Brick").getSize());
                //t->addComponent<CTransform>(gridToMidPixel((float)m_mousePos.gridPos.x, (float)m_mousePos.gridPos.y, t));
                //t->addComponent<CDraggable>();
                //t->getComponent<CDraggable>().dragging = true;
            }
            ImGui::SameLine();
            if (ImGui::ImageButton("Ground", ground, size, sf::Color::Transparent, sf::Color(255, 255, 255)))
            {
                auto t = m_entityManager->addEntity("Tile");
                t->addComponent<CAnimation>(m_game->assets().getAnimation("Ground"), true);
                t->addComponent<CBoundingBox>(m_game->assets().getAnimation("Ground").getSize());
                //t->addComponent<CTransform>(gridToMidPixel((float)m_mousePos.gridPos.x, (float)m_mousePos.gridPos.y, t));
                //t->addComponent<CDraggable>();
                //t->getComponent<CDraggable>().dragging = true;
            }
            ImGui::SameLine();
            if (ImGui::ImageButton("Block", block, size, sf::Color::Transparent, sf::Color(255, 255, 255)))
            {
                auto t = m_entityManager->addEntity("Tile");
                t->addComponent<CAnimation>(m_game->assets().getAnimation("Block"), true);
                t->addComponent<CBoundingBox>(m_game->assets().getAnimation("Block").getSize());
                //t->addComponent<CTransform>(gridToMidPixel((float)m_mousePos.gridPos.x, (float)m_mousePos.gridPos.y, t));
                //t->addComponent<CDraggable>();
                //t->getComponent<CDraggable>().dragging = true;
            }
            ImGui::SameLine();
            if (ImGui::ImageButton("Basalt", m_game->assets().getAnimation("Basalt").getSprite(),
                size, sf::Color::Transparent, sf::Color(255, 255, 255)))
            {
                auto t = m_entityManager->addEntity("Tile");
                t->addComponent<CAnimation>(m_game->assets().getAnimation("Basalt"), true);
                t->addComponent<CBoundingBox>(m_game->assets().getAnimation("basalt").getSize());
                //t->addComponent<CTransform>(gridToMidPixel((float)m_mousePos.gridPos.x, (float)m_mousePos.gridPos.y, t));
                //t->addComponent<CDraggable>();
                //t->getComponent<CDraggable>().dragging = true;
            }
            ImGui::SameLine();
            if (ImGui::ImageButton("QuestionFull", m_game->assets().getAnimation("QuestionFull").getSprite(),
                size, sf::Color::Transparent, sf::Color(255, 255, 255)))
            {
                auto t = m_entityManager->addEntity("Tile");
                t->addComponent<CAnimation>(m_game->assets().getAnimation("QuestionFull"), true);
                t->addComponent<CBoundingBox>(m_game->assets().getAnimation("QuestionFull").getSize());
                //t->addComponent<CTransform>(gridToMidPixel((float)m_mousePos.gridPos.x, (float)m_mousePos.gridPos.y, t));
                //t->addComponent<CDraggable>();
                //t->getComponent<CDraggable>().dragging = true;
            }

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Decorations"))
        {
            ImGui::EndTabItem();
        }

        drawEntityManagerMenu();

        ImGui::EndTabBar();
    }
}

void Menu::renderMenu(sf::View view)
{
}
void Menu::drawEntityManagerMenu()
{
    if (ImGui::BeginTabItem("Entities"))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
        ImGui::BeginChild("EntityList", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), ImGuiChildFlags_None, window_flags);

        ImGui::Columns(5, "entitycolumns");
        ImGui::SetColumnWidth(0, 100);
        ImGui::SetColumnWidth(1, 50);
        ImGui::SetColumnWidth(2, 100);
        //ImGui::SetColumnWidth(3, 75);

        ImGui::Separator();
        ImGui::Text("Id"); ImGui::NextColumn();
        ImGui::Text("D"); ImGui::NextColumn();
        ImGui::Text("Pos"); ImGui::NextColumn();
        ImGui::Text("Tag"); ImGui::NextColumn();
        ImGui::Text("Name"); ImGui::NextColumn();
        ImGui::Separator();

        for (auto& e : m_entityManager->getEntities())
        {
            ImGui::Text("%04d", e->id());
            ImGui::NextColumn();
            if (ImGui::ImageButton("Entity##", e->getComponent<CAnimation>().animation.getSprite(),
                sf::Vector2f(24, 24), sf::Color::Transparent, sf::Color(255, 255, 255)))
            {
                //TODO: edit box for entity properties
            }
            ImGui::NextColumn();
            Vec2 pos = pixelToMidGrid(e->getComponent<CTransform>().pos.x, e->getComponent<CTransform>().pos.y, e);
            ImGui::Text("%d %d", (int)pos.x, (int)pos.y);
            ImGui::NextColumn();
            ImGui::Text("%s", e->tag().c_str());
            ImGui::NextColumn();
            ImGui::Text("%s", e->getComponent<CAnimation>().animation.getName().c_str());
            ImGui::NextColumn();
        }
        ImGui::EndChild();
        ImGui::PopStyleVar();
        ImGui::EndTabItem();
    }

}
#define ICON_MD_CONTENT_COPY "\xee\x85\x8d"	// U+e14d
#define ICON_MD_COMPUTER "\xee\x8c\x8a"	// U+e30a

void Menu::DrawStatusPopup()
{
    
}

void Menu::drawSaveAsWindow()
{
    if (m_displaySaveWindow)
    {
        if (BeginCentered("Save As")) {
            ImGui::Spacing();
            ImGui::NextColumn();
            ImGui::Columns(1);
            ImGui::Separator();
            ImGui::NewLine();
            ImGui::SameLine(270);
            if (ImGui::Button("OK")) {

                m_displaySaveWindow = !m_displaySaveWindow;
            }
            ImGui::SameLine();
            if (ImGui::Button(ICON_MD_CONTENT_COPY, ImVec2(50, 0))) {
            }
            ImGui::End();
        }
    }
}