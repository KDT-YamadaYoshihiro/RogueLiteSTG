// ScreenTitle.cpp
#include "ScreenTitle.h"
#include "ScreenManager.h"
#include "../App/CApp.h"

void ScreenTitle::Initialize(App* app)
{
    m_app = app;
}

void ScreenTitle::Update()
{
    // Enterキーでゲーム開始
    if (GetAsyncKeyState(VK_RETURN) & 0x8000)
    {
        m_app->GetScreenManager()->ChangeScreen(ScreenType::INGAME, m_app);
    }
}

void ScreenTitle::Render()
{
    // ここでテキスト描画（後でスプライト描画クラスを追加）
}

void ScreenTitle::Finalize()
{
}
