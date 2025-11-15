// ScreenBase.h
#pragma once

class App; // ‘O•ûéŒ¾

class ScreenBase
{
public:
	virtual ~ScreenBase() = default;

    virtual void OnEnter() {}         // ‰æ–Ê‚É“ü‚Á‚½‚Æ‚«
    virtual void OnExit() {}          // ‰æ–Ê‚©‚ço‚é‚Æ‚«

    virtual void Initialize() = 0;

    virtual void Update() = 0;
    virtual void Render() = 0;

    virtual void Finalize() = 0;
};
