#pragma once
#include <Windows.h>

namespace user_interface {
	void initiate(HWND hwnd);

    LRESULT WINAPI window_proc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);

	extern int width;
	extern int height;
};