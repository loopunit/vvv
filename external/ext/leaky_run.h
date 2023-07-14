/**
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * Copyright 2017-present Rodrigo Cesar de Freitas Dias
 * This library is released under the MIT License
 */

#pragma once
#include <crtdbg.h>
#include <Windows.h>
#include <CommCtrl.h>
#pragma comment(lib, "Comctl32.lib")

namespace wl {
namespace _wli {

template<typename wnd_mainT, typename... T_ARGS>
int leaky_run_main(HINSTANCE hInst, int cmdShow, T_ARGS... args) noexcept {
	int ret = 0;
	try { // any exception which was not caught, except those from within message lambdas
		wnd_mainT wndMain(args...);
		ret = wndMain.winmain_run(hInst, cmdShow);
	} catch (...) {
		{
			const char* caption = "Unknown error";
			const char* msg = "An unknown exception, not derived from std::exception, was thrown.";

			// https://stackoverflow.com/a/48036486/6923555
			try { throw; }
			catch (const std::invalid_argument& e) { msg = e.what(); caption = "Invalid argument error"; }
			catch (const std::out_of_range& e)     { msg = e.what(); caption = "Out of range error"; }
			catch (const std::logic_error& e)      { msg = e.what(); caption = "Logic error"; }
			catch (const std::system_error& e)     { msg = e.what(); caption = "System error"; }
			catch (const std::runtime_error& e)    { msg = e.what(); caption = "Runtime error"; }
			catch (const std::exception& e)        { msg = e.what(); caption = "Generic error"; }

			MessageBoxA(nullptr, msg, caption, MB_ICONERROR);
		}
		ret = -1;
	}
	//_ASSERT(!_CrtDumpMemoryLeaks());
	return ret;
}

}//namespace _wli
}//namespace wl
