#include "window.h"
#include "../user_interface/user_interface.h"

HWND window::initiate() {
    WNDCLASSEX wc = {
        sizeof(WNDCLASSEX),
        CS_CLASSDC,
        user_interface::window_proc,
        0L,
        0L,
        GetModuleHandleA(nullptr),
        NULL,
        NULL,
        NULL,
        NULL,
        L"Window Base",
        NULL
    };
    RegisterClassEx(&wc);
    HWND hwnd = CreateWindow(wc.lpszClassName, L"Window Base", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    RECT window_rect;
    GetWindowRect(hwnd, &window_rect);
    user_interface::width = window_rect.right - window_rect.left;
    user_interface::height = window_rect.bottom - window_rect.top;

    return hwnd;
}