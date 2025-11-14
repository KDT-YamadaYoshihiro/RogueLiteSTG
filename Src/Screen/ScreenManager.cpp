// ScreenManager.cpp
#include "ScreenManager.h"
#include "ScreenTitle.h"
#include "ScreenInGame.h"
#include "ScreenResult.h"

ScreenManager::ScreenManager() {}
ScreenManager::~ScreenManager()
{
    if (m_current) m_current->Finalize();
    delete m_current;
}

void ScreenManager::ChangeScreen(ScreenType next, App* app)
{
    if (m_current) {
        m_current->Finalize();
        delete m_current;
    }

    switch (next)
    {
    case ScreenType::TITLE:
        m_current = new ScreenTitle();
        break;
    case ScreenType::INGAME:
        m_current = new ScreenInGame();
        break;
    case ScreenType::RESULT:
        m_current = new ScreenResult();
        break;
    }

    m_current->Initialize(app);
}

void ScreenManager::Update()
{
    if (m_current) m_current->Update();
}

void ScreenManager::Render()
{
    if (m_current) m_current->Render();
}
