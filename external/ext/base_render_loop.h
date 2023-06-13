/**
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * Copyright 2017-present Rodrigo Cesar de Freitas Dias
 * This library is released under the MIT License
 */

#pragma once
#include <system_error>
#include <vector>
#include <Windows.h>

namespace wl {
namespace _wli {

// Wraps the main program loop.
class base_render_loop final {
private:
	std::vector<HWND> _modelessChildren;
	UINT32 _renderLoopMessage;
public:
	base_render_loop(UINT32 renderLoopMessage) : _renderLoopMessage(renderLoopMessage) {}

	int run_loop(HWND hWnd, HACCEL hAccel = nullptr) const {
		MSG msg{};

		bool done = false;
		while (!done)
		{
			// Poll and handle messages (inputs, window resize, etc.)
			// See the WndProc() function below for our to dispatch events to the Win32 backend.
			while (PeekMessageW(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				// NOTE: this ends up blocking WM_CHAR
				//if (this->_is_modeless_msg(&msg) || // http://www.winprog.org/tutorial/modeless_dialogs.html
				//	(hAccel && TranslateAcceleratorW(hWnd, hAccel, &msg)) ||
				//	IsDialogMessageW(hWnd, &msg) ) 
				//{
				//	continue;
				//}
				
				TranslateMessage(&msg);
				DispatchMessageW(&msg);

				if (msg.message == WM_QUIT)
				{
					done = true;
				}
			}

			SendMessageW(
				hWnd,
				_renderLoopMessage,
				0,
				0);

			if (done)
				break;		
		}
		
		return static_cast<int>(msg.wParam); // this can be used as program return value
	}

	void add_modeless(HWND hWnd) {
		this->_modelessChildren.emplace_back(hWnd);
	}

	void remove_modeless(HWND hWnd) {
		for (std::vector<HWND>::iterator it = this->_modelessChildren.begin();
			it != this->_modelessChildren.end(); ++it)
		{
			if (*it == hWnd) {
				this->_modelessChildren.erase(it);
				break;
			}
		}
	}

private:
	bool _is_modeless_msg(MSG* pMsg) const noexcept {
		for (const HWND hModl : this->_modelessChildren) {
			if (!hModl || !IsWindow(hModl)) continue; // skip invalid HWND
			if (IsDialogMessageW(hModl, pMsg)) return true;
		}
		return false;
	}
};

}//namespace _wli
}//namespace wl
