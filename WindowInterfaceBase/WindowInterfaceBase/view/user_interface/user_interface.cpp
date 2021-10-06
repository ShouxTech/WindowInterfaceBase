#include "user_interface.h"
#include "../imgui/imgui.h"
#include "../imgui/backends/imgui_impl_win32.h"
#include "../imgui/backends/imgui_impl_dx11.h"
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);

static ID3D11Device* device = nullptr;
static ID3D11DeviceContext* device_context = nullptr;
static IDXGISwapChain* swapchain = nullptr;
static ID3D11RenderTargetView* main_render_target_view = nullptr;

bool create_device_d3d(HWND hwnd);
void cleanup_device_d3d();
void create_render_target();
void cleanup_render_target();

int user_interface::width = 100;
int user_interface::height = 100;

void user_interface::initiate(HWND hwnd) {
    if (!create_device_d3d(hwnd))     {
        cleanup_device_d3d();
        return;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device, device_context);

    float clear[] = { 0, 0, 0, 0 };
    float value = 0;

    while (true) {
        {
            MSG msg;
            while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                if (msg.message == WM_QUIT) {
                    break;
                }
            }
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(user_interface::width, user_interface::height));

        {
            ImGui::Begin("Interface Base", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
            ImGui::BeginTabBar("Tabs");

            if (ImGui::BeginTabItem("Primary")) {
                ImGui::Text("Label");
                ImGui::SliderFloat("Value", &value, 0, 500);

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Secondary")) {
                if (ImGui::Button("Click")) {
                    // Do something when clicked.
                }

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
            ImGui::End();
        }

        ImGui::Render();

        device_context->OMSetRenderTargets(1, &main_render_target_view, NULL);
        device_context->ClearRenderTargetView(main_render_target_view, clear);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        swapchain->Present(1, 0); // Present with VSync.
        //swapchain->Present(0, 0); // Present without VSync.
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    cleanup_device_d3d();
    DestroyWindow(hwnd);

    exit(0);
}

LRESULT __stdcall user_interface::window_proc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) {
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, w_param, l_param)) {
        return true;
    }

    switch (msg) {
    case WM_SIZE:
        if (device != NULL && w_param != SIZE_MINIMIZED) {
            cleanup_render_target();
            UINT width = LOWORD(l_param);
            UINT height = HIWORD(l_param);
            user_interface::width = width;
            user_interface::height = height;
            swapchain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
            create_render_target();
        }
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        exit(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, w_param, l_param);
}

bool create_device_d3d(HWND hwnd) {
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT create_device_flags = 0;
    D3D_FEATURE_LEVEL feature_level;
    const D3D_FEATURE_LEVEL feature_level_array[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, create_device_flags, feature_level_array, 2, D3D11_SDK_VERSION, &sd, &swapchain, &device, &feature_level, &device_context) != S_OK)
        return false;

    create_render_target();
    return true;
}

void cleanup_device_d3d() {
    cleanup_render_target();
    if (swapchain) {
        swapchain->Release();
        swapchain = NULL;
    }
    if (device_context) {
        device_context->Release();
        device_context = NULL;
    }
    if (device) {
        device->Release();
        device = NULL;
    }
}

void create_render_target() {
    ID3D11Texture2D* pBackBuffer;
    swapchain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    device->CreateRenderTargetView(pBackBuffer, NULL, &main_render_target_view);
    pBackBuffer->Release();
}

void cleanup_render_target() {
    if (main_render_target_view) {
        main_render_target_view->Release();
        main_render_target_view = NULL;
    }
}