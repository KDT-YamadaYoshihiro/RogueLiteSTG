#include "CApp.h"
#include <stdexcept>

using Microsoft::WRL::ComPtr;

// ------------------------------------------
// 初期化
// ------------------------------------------
bool App::Initialize(HWND hwnd, int width, int height)
{
    m_width = width;
    m_height = height;

    m_screenManager = std::make_unique<ScreenManager>();
    m_screenManager->ChangeScreen(ScreenType::TITLE, this);


    return InitDX12(hwnd);
}

// ------------------------------------------
// DirectX12 初期化
// ------------------------------------------
bool App::InitDX12(HWND hwnd)
{
    // ---------------------------------------------------
    // 1. DXGIファクトリ作成
    // ---------------------------------------------------
    ComPtr<IDXGIFactory6> factory;
    if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory)))) return false;

    // ---------------------------------------------------
    // 2. デバイス作成
    // ---------------------------------------------------
    if (FAILED(D3D12CreateDevice(
        nullptr,
        D3D_FEATURE_LEVEL_11_0,
        IID_PPV_ARGS(&m_device)
    ))) return false;

    // ---------------------------------------------------
    // 3. コマンドキュー作成
    // ---------------------------------------------------
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    if (FAILED(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue))))
        return false;

    // ---------------------------------------------------
    // 4. スワップチェーン作成
    // ---------------------------------------------------
    DXGI_SWAP_CHAIN_DESC1 swapDesc = {};
    swapDesc.BufferCount = FrameCount;
    swapDesc.Width = m_width;
    swapDesc.Height = m_height;
    swapDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> tmpSwapChain;
    if (FAILED(factory->CreateSwapChainForHwnd(
        m_commandQueue.Get(),
        hwnd,
        &swapDesc,
        nullptr,
        nullptr,
        &tmpSwapChain)))
        return false;

    tmpSwapChain.As(&m_swapChain);
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

    // ---------------------------------------------------
    // 5. RTVディスクリプタヒープ作成
    // ---------------------------------------------------
    D3D12_DESCRIPTOR_HEAP_DESC rtvDesc = {};
    rtvDesc.NumDescriptors = FrameCount;
    rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

    if (FAILED(m_device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&m_rtvHeap))))
        return false;

    m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    // ---------------------------------------------------
    // 6. バックバッファにRTV割り当て
    // ---------------------------------------------------
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();

    for (UINT i = 0; i < FrameCount; i++)
    {
        if (FAILED(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]))))
            return false;

        m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);

        rtvHandle.ptr += m_rtvDescriptorSize;
    }

    // ---------------------------------------------------
    // 7. コマンドアロケータ & コマンドリスト作成
    // ---------------------------------------------------
    if (FAILED(m_device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&m_commandAllocator))))
        return false;

    if (FAILED(m_device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        m_commandAllocator.Get(),
        nullptr,
        IID_PPV_ARGS(&m_commandList))))
        return false;

    m_commandList->Close();

    // ---------------------------------------------------
    // 8. フェンス作成
    // ---------------------------------------------------
    if (FAILED(m_device->CreateFence(
        0,
        D3D12_FENCE_FLAG_NONE,
        IID_PPV_ARGS(&m_fence))))
        return false;

    m_fenceValue = 1;

    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (m_fenceEvent == nullptr)
        return false;

    return true;
}

// --------------------------------------------------
// 毎フレーム更新
// --------------------------------------------------
void App::Update()
{
    m_screenManager->Update();
}

// --------------------------------------------------
// 描画（黒画面）
// --------------------------------------------------
void App::Render()
{
    // -----------------------------
    // 1. コマンド再利用準備
    // -----------------------------
    m_commandAllocator->Reset();
    m_commandList->Reset(m_commandAllocator.Get(), nullptr);

    // -----------------------------
    // 2. リソースバリア：Present → RenderTarget
    // -----------------------------
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = m_renderTargets[m_frameIndex].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    m_commandList->ResourceBarrier(1, &barrier);

    // -----------------------------
    // 3. RTV設定
    // -----------------------------
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
    rtvHandle.ptr += m_frameIndex * m_rtvDescriptorSize;

    const float clearColor[] = { 0.1f, 0.1f, 0.12f, 1.0f };
    m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    // -----------------------------
    // 4. リソースバリア：RenderTarget → Present
    // -----------------------------
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

    m_commandList->ResourceBarrier(1, &barrier);

    // -----------------------------
    // 5. コマンドリスト実行
    // -----------------------------
    m_commandList->Close();
    ID3D12CommandList* cmds[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(1, cmds);

	// 画面描画
    m_screenManager->Render();

    // -----------------------------
    // 6. Present
    // -----------------------------
    m_swapChain->Present(1, 0);

    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

    WaitForGPU();
}

// --------------------------------------------------
// GPU完了待ち
// --------------------------------------------------
void App::WaitForGPU()
{
    m_commandQueue->Signal(m_fence.Get(), m_fenceValue);

    if (m_fence->GetCompletedValue() < m_fenceValue)
    {
        m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent);
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }

    m_fenceValue++;
}

// --------------------------------------------------
// 終了処理
// --------------------------------------------------
void App::Terminate()
{
    WaitForGPU();

    CloseHandle(m_fenceEvent);
}