#include <windows.h>
#include "CApp.h"

// -----------------------------
// ウィンドウプロシージャ
// -----------------------------
LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

// -----------------------------
// エントリーポイント
// -----------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    // ウィンドウクラス登録
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"DX12GameWindowClass";

    if (!RegisterClassEx(&wc)) {
        MessageBoxA(nullptr, "ウィンドウクラス登録に失敗しました", "Error", MB_OK);
        return -1;
    }

    // ウィンドウサイズ
    const int width = 1280;
    const int height = 720;

    RECT wr = { 0, 0, width, height };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    // ウィンドウ生成
    HWND hWnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        L"RogueLiteSTG",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        wr.right - wr.left,
        wr.bottom - wr.top,
        nullptr, nullptr, hInstance, nullptr
    );

    if (!hWnd) {
        MessageBoxA(nullptr, "ウィンドウ作成に失敗しました", "Error", MB_OK);
        return -1;
    }

    ShowWindow(hWnd, nCmdShow);

    // -----------------------------
    // DirectX12アプリ初期化
    // -----------------------------
    App app;
    if (!app.Initialize(hWnd, width, height)) {
        MessageBoxA(nullptr, "DirectX12 初期化に失敗しました", "Error", MB_OK);
        return -1;
    }

    // -----------------------------
    // メインループ
    // -----------------------------
    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        // Windows メッセージ処理
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // アプリの更新と描画
            app.Update();
            app.Render();
        }
    }

    // -----------------------------
    // 終了処理
    // -----------------------------
    app.Terminate();

    return 0;
}
