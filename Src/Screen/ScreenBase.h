// ScreenBase.h
#pragma once

class App; // ‘O•ûéŒ¾

class ScreenBase
{
public:
    virtual ~ScreenBase() {}

    virtual void Initialize(App* app) = 0;
    virtual void Update() = 0;
    virtual void Render() = 0;
    virtual void Finalize() = 0;
};
