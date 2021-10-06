#include <iostream>
//#include <thread>
#include <Windows.h>
#include "../view/window/window.h"
#include "../view/user_interface/user_interface.h"

int APIENTRY WinMain(HINSTANCE hinstance, HINSTANCE hinst_prev, PSTR cmdline, int cmdshow) {
	HWND hwnd;
	//std::thread interface_thread = std::thread([&] {
	hwnd = window::initiate();
	user_interface::initiate(hwnd); // Note: This function is blocking. It can be moved inside a seperate
									// thread to stop it from blocking any subsequent code.
	//});
}