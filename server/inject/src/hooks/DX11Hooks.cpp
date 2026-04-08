#include "hooks/DX11Hooks.hpp"

#include <Windows.h>
#include <d3d11.h>
#include <D3Dcompiler.h>
#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx11.h>
#include <MinHook.h>

#include "panel/ServerPanel.hpp"
#include "panel/VotesPanel.hpp"

typedef HRESULT(__stdcall* Present_t)(IDXGISwapChain*, UINT, UINT);
typedef HRESULT(__stdcall* ResizeBuffers_t)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);

Present_t       oPresent       = nullptr;
ResizeBuffers_t oResizeBuffers = nullptr;

ID3D11Device*           g_device    = nullptr;
ID3D11DeviceContext*    g_context   = nullptr;
ID3D11RenderTargetView* g_rtv       = nullptr;
HWND                    g_hwnd      = nullptr;
bool                    g_imguiInit = false;
bool                    g_visible   = true;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

WNDPROC g_origWndProc = nullptr;
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (g_visible) {
        ImGui_ImplWin32_WndProcHandler(hwnd, msg, wp, lp);
        
        if (ImGui::GetIO().WantCaptureMouse) return true;
    }
    return CallWindowProc(g_origWndProc, hwnd, msg, wp, lp);
}

void InitImGui(IDXGISwapChain* swapChain) {
    swapChain->GetDevice(__uuidof(ID3D11Device), (void**)&g_device);
    g_device->GetImmediateContext(&g_context);
    
    DXGI_SWAP_CHAIN_DESC desc;
    swapChain->GetDesc(&desc);
    g_hwnd = desc.OutputWindow;
    
    ID3D11Texture2D* backBuffer = nullptr;
    swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    g_device->CreateRenderTargetView(backBuffer, nullptr, &g_rtv);
    backBuffer->Release();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDrawCursor = g_visible;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(g_hwnd);
    ImGui_ImplDX11_Init(g_device, g_context);

    g_origWndProc = (WNDPROC)SetWindowLongPtr(g_hwnd, GWLP_WNDPROC, (LONG_PTR)WndProc);

    g_imguiInit = true;
}


HRESULT __stdcall hkPresent(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) {
    if (!g_imguiInit)
        InitImGui(swapChain);

    if (GetAsyncKeyState(VK_INSERT) & 1)
        g_visible = !g_visible;

    if (g_visible) {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ServerPanel::get()->renderHostPanel();
        VotesPanel::get()->render();

        ImGui::Render();
        g_context->OMSetRenderTargets(1, &g_rtv, nullptr);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    return oPresent(swapChain, syncInterval, flags);
}


HRESULT __stdcall hkResizeBuffers(
    IDXGISwapChain* swapChain, UINT count,
    UINT w, UINT h, DXGI_FORMAT fmt, UINT flags)
{
    if (g_rtv) { g_rtv->Release(); g_rtv = nullptr; }
    HRESULT hr = oResizeBuffers(swapChain, count, w, h, fmt, flags);
    ID3D11Texture2D* backBuffer = nullptr;
    swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    g_device->CreateRenderTargetView(backBuffer, nullptr, &g_rtv);
    backBuffer->Release();
    return hr;
}

void createDX11Hooks() {
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = GetForegroundWindow(); 
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    ID3D11Device* dummyDevice = nullptr;
    IDXGISwapChain* dummySwapChain = nullptr;

    D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        nullptr, 0, D3D11_SDK_VERSION,
        &sd, &dummySwapChain, &dummyDevice,
        nullptr, nullptr
    );

    
    void** vmt = *reinterpret_cast<void***>(dummySwapChain);

    MH_CreateHook(vmt[8],  &hkPresent,       (void**)&oPresent);
    MH_CreateHook(vmt[13], &hkResizeBuffers, (void**)&oResizeBuffers);

    MH_EnableHook(vmt[8]);
    MH_EnableHook(vmt[13]);

    dummyDevice->Release();
    dummySwapChain->Release();
}