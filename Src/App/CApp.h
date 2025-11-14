#pragma once
#include <windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <memory>
#include "../Screen/ScreenManager.h"

class App
{
public:
    ScreenManager* GetScreenManager() { return m_screenManager.get(); }
    bool Initialize(HWND hwnd, int width, int height);
    void Update();
    void Render();
    void Terminate();

private:
    bool InitDX12(HWND hwnd);
    void WaitForGPU();
    std::unique_ptr<ScreenManager> m_screenManager;

private:
    int m_width = 0;
    int m_height = 0;

    Microsoft::WRL::ComPtr<ID3D12Device>            m_device;
    Microsoft::WRL::ComPtr<IDXGISwapChain3>         m_swapChain;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue>      m_commandQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator>  m_commandAllocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

    static const UINT FrameCount = 2;
    Microsoft::WRL::ComPtr<ID3D12Resource>          m_renderTargets[FrameCount];

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>    m_rtvHeap;
    UINT m_rtvDescriptorSize = 0;

    Microsoft::WRL::ComPtr<ID3D12Fence>             m_fence;
    UINT64 m_fenceValue = 0;
    HANDLE m_fenceEvent = nullptr;

    UINT m_frameIndex = 0;
};
