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
    // 初期化
    ScreenManager();
	// 終了
    ~ScreenManager() = default;


public:

    //　シングルトン
	ScreenManager(const ScreenManager&) = delete;
	ScreenManager& operator=(const ScreenManager&) = delete;
    static ScreenManager& GetInstance()
    {
        static ScreenManager instance;
        return instance;
	}

    void ChangeScreen(ScreenType type);

    void Update();
    void Render();

    template<typename T>
    void RegisterScreen(ScreenType id)
    {
        m_screens[id] = std::make_unique<T>();
    }



private:
	ScreenType m_currentType;
    std::unique_ptr<ScreenBase>* m_current = nullptr;
	std::unordered_map<ScreenType, std::unique_ptr<ScreenBase>> m_screens;
};
