#pragma once
#include <memory>
#include <unordered_map>

#include "ScreenBase.h"


enum class ScreenType
{
    TITLE,
    INGAME,
    RESULT
};

class ScreenManager
{
public:
    ScreenManager();
    ~ScreenManager();

    void ChangeScreen(ScreenType next, class App* app);

    void Update();
    void Render();

private:
    ScreenBase* m_current = nullptr;
};
