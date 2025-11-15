// ScreenTitle.cpp
#include "ScreenTitle.h"
#include "ScreenManager.h"
#include "../App/CApp.h"

void ScreenTitle::Initialize()
{
}

void ScreenTitle::Update()
{
    // Enterキーでゲーム開始
    if (GetAsyncKeyState(VK_RETURN) & 0x8000)
    {
		ScreenManager::GetInstance().ChangeScreen(ScreenType::INGAME);
    }
}

void ScreenTitle::Render()
{
    // ここでテキスト描画（後でスプライト描画クラスを追加）
}

void ScreenTitle::Finalize()
{
}
