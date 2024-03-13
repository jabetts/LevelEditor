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

void Menu::setScene(Scene_Play* scene)
{
    m_scene = scene;
}

void Menu::drawMainMenu(Flags& flags)
{
    ImGui::Begin("Level editor", &m_showWindow, ImGuiWindowFlags_MenuBar);
    
    ImGui::Text("Display");
    ImGui::Checkbox("Grid", &flags.drawGrid);
    ImGui::SameLine();
    ImGui::Checkbox("Collisions", &flags.drawCollisions);
    ImGui::SameLine();
    ImGui::Checkbox("Textures", &flags.drawTextures);
    ImGui::Separator();

    drawMenuBar();
   
    //drawTabs();
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
            m_scene->saveLevel(m_scene->filenameBuffer);
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
//       or sprite we want based on the type. i.e. Dec_ prefix
void Menu::drawTabs()
{
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

    if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
    {
        auto tiles = m_game->assets().getAnimationMap();

        if (ImGui::BeginTabItem("Tiles"))
        {
            //Manual button wrapping
            ImGuiStyle& style = ImGui::GetStyle();
            int buttons_count = tiles.size();
            float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
            int n = 0;

            for (const auto& [name, animation] : tiles)
            {
                Animation anim = animation;

                if (std::strncmp(name.c_str(), "Dec", 3) == 0)
                {
                    continue;
                }

                sf::Texture& tex = m_game->assets().getTexture(name);
                Vec2 size = anim.getSize();

                if (ImGui::ImageButton(anim.getName().c_str(), anim.getSprite(), sf::Vector2f(size.x, size.y), sf::Color::Transparent,
                    sf::Color(255, 255, 255, 255)))
                {
                    //auto t = m_entityManager.addEntity("Tile");
                    //std::cout << name << " pressed\n";
                    //t->addComponent<CAnimation>(anim, true);
                    //t->addComponent<CBoundingBox>(anim.getSize());
                    //t->addComponent<CTransform>(gridToMidPixel((float)m_mousePos.gridPos.x, (float)m_mousePos.gridPos.y, t));
                    //t->addComponent<CDraggable>();
                    //t->getComponent<CDraggable>().dragging = true;
                }

                float last_button_x2 = ImGui::GetItemRectMax().x;
                float next_button_x2 = last_button_x2 + style.ItemSpacing.x + size.x; // Expected position if next button was on same line
                if (n + 1 < buttons_count && next_button_x2 < window_visible_x2)
                {
                    ImGui::SameLine();
                }

                n++;
            }

            ImGui::EndTabItem();
        }
        //if (ImGui::BeginTabItem("Decorations"))
        //{
        //    ImGui::EndTabItem();
        //}

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
            char idtext[32];
            std::snprintf(idtext, sizeof(idtext), "%d", e->id());
            ImGui::NextColumn();
            if (ImGui::ImageButton(idtext, e->getComponent<CAnimation>().animation.getSprite(),
                sf::Vector2f(24, 24), sf::Color::Transparent, sf::Color(255, 255, 255)))
            {
                e->destroy();
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