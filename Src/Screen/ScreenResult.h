// ScreenTitle.h
#pragma once
#include "ScreenBase.h"

class ScreenResult : public ScreenBase
{
public:
    void Initialize() override;
    void Update() override;
    void Render() override;
    void Finalize() override;

private:
    App* m_app = nullptr;
};
