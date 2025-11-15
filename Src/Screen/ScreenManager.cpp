#include "ScreenManager.h"
#include "ScreenTitle.h"
#include "ScreenInGame.h"
#include "ScreenResult.h"


ScreenManager::ScreenManager()
    : m_currentType(ScreenType::TITLE)
{
    // Še‰æ–Ê‚ð“o˜^
    RegisterScreen<ScreenTitle>(ScreenType::TITLE);
    RegisterScreen<ScreenInGame>(ScreenType::INGAME);
    RegisterScreen<ScreenResult>(ScreenType::RESULT);
    // Å‰‚Ì‰æ–Ê‚ðÝ’è
    ChangeScreen(m_currentType);
}

void ScreenManager::ChangeScreen(ScreenType id)
{
    if (m_current)
        (*m_current)->OnExit();

    m_currentType = id;
    m_current = &m_screens[id];

    (*m_current)->OnEnter();
}

void ScreenManager::Update()
{
    if (m_current)
        (*m_current)->Update();
}

void ScreenManager::Render()
{
    if (m_current)
        (*m_current)->Render();
}
