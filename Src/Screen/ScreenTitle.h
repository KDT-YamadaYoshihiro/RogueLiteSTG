// ScreenTitle.h
#pragma once
#include "ScreenBase.h"

class ScreenTitle : public ScreenBase
{
public:
    void Initialize(App* app) override;
    void Update() override;
    void Render() override;
    void Finalize() override;

private:
    App* m_app = nullptr;
};
