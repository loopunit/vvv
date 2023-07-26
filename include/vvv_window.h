#pragma once

#include <resizer.h>
#include <render_window_main.h>
#include <leaky_run.h>

#include <vvv.h>

class root_window : public wl::render_window_main
{
public:
	std::shared_ptr<vvv::vk_instance>		 m_vk_instance;
	std::shared_ptr<vvv::vk_physical_device> m_vk_physical_device;
	std::shared_ptr<vvv::vk_queue_families>	 m_vk_queue_families;
	std::shared_ptr<vvv::vk_surface>		 m_vk_surface;
	std::shared_ptr<vvv::vk_device>			 m_vk_device;
	std::shared_ptr<vvv::vk_swapchain>		 m_vk_swapchain;

	INT64  m_last_time{0};
	INT64  m_ticks_per_second{0};
	HWND   m_mouse_hwnd{0};
	bool   m_mouse_tracked{false};
	int	   m_mouse_buttons_down{0};
	RECT   m_rect{0, 0, 0, 0};
	double m_delta_time{0.0};

	// ImGuiMouseCursor m_last_mouse_cursor{ImGuiMouseCursor_COUNT};

	// static inline constexpr int IM_VK_KEYPAD_ENTER = (VK_RETURN + 256);

	template<UINT msg>
	void mouse_button_down(auto wParam)
	{
		int button = 0;
		if constexpr (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK)
		{
			button = 0;
		}
		else if constexpr (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK)
		{
			button = 1;
		}
		else if constexpr (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK)
		{
			button = 2;
		}
		else if constexpr (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK)
		{
			button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4;
		}

		if (m_mouse_buttons_down == 0 && ::GetCapture() == NULL)
		{
			::SetCapture(hwnd());
		}

		m_mouse_buttons_down |= 1 << button;

		// ImGui::GetIO().AddMouseButtonEvent(button, true);
	}

	template<UINT msg>
	void mouse_button_up(auto wParam)
	{
		int button = 0;
		if constexpr (msg == WM_LBUTTONUP)
		{
			button = 0;
		}
		else if constexpr (msg == WM_RBUTTONUP)
		{
			button = 1;
		}
		else if constexpr (msg == WM_MBUTTONUP)
		{
			button = 2;
		}
		else if constexpr (msg == WM_XBUTTONUP)
		{
			button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4;
		}

		m_mouse_buttons_down &= ~(1 << button);

		if (m_mouse_buttons_down == 0 && ::GetCapture() == hwnd())
		{
			::ReleaseCapture();
		}

		// ImGui::GetIO().AddMouseButtonEvent(button, false);
	}

	bool update_mouse_cursor()
	{
#if 0
		ImGuiIO& io = ImGui::GetIO();

		if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
		{
			return false;
		}

		if (ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor(); (imgui_cursor == ImGuiMouseCursor_None) || io.MouseDrawCursor)
		{
			::SetCursor(NULL);
		}
		else
		{
			auto load_cursor = [](LPTSTR win32_cursor)
			{
				::SetCursor(::LoadCursor(NULL, win32_cursor));
			};
			switch (imgui_cursor)
			{
			case ImGuiMouseCursor_Arrow:
				load_cursor(IDC_ARROW);
				break;
			case ImGuiMouseCursor_TextInput:
				load_cursor(IDC_IBEAM);
				break;
			case ImGuiMouseCursor_ResizeAll:
				load_cursor(IDC_SIZEALL);
				break;
			case ImGuiMouseCursor_ResizeEW:
				load_cursor(IDC_SIZEWE);
				break;
			case ImGuiMouseCursor_ResizeNS:
				load_cursor(IDC_SIZENS);
				break;
			case ImGuiMouseCursor_ResizeNESW:
				load_cursor(IDC_SIZENESW);
				break;
			case ImGuiMouseCursor_ResizeNWSE:
				load_cursor(IDC_SIZENWSE);
				break;
			case ImGuiMouseCursor_Hand:
				load_cursor(IDC_HAND);
				break;
			case ImGuiMouseCursor_NotAllowed:
				load_cursor(IDC_NO);
				break;
			default:
				load_cursor(IDC_ARROW);
				break;
			}
		}
		return true;
#endif
		return false;
	}

	void update_mouse_data()
	{
		// ImGuiIO& io = ImGui::GetIO();
		//
		//// If is focused
		// if (::GetForegroundWindow() == hwnd())
		//{
		//	if (io.WantSetMousePos)
		//	{
		//		if (POINT pos = {(int)io.MousePos.x, (int)io.MousePos.y}; ::ClientToScreen(hwnd(), &pos))
		//		{
		//			::SetCursorPos(pos.x, pos.y);
		//		}
		//	}
		//
		//	if (!io.WantSetMousePos && !m_mouse_tracked)
		//	{
		//		if (POINT pos; ::GetCursorPos(&pos) && ::ScreenToClient(hwnd(), &pos))
		//		{
		//			io.AddMousePosEvent((float)pos.x, (float)pos.y);
		//		}
		//	}
		// }
	}

	template<bool is_key_down>
	auto handle_key(WPARAM wParam, LPARAM lParam)
	{
		if constexpr (is_key_down)
		{
			// m_sample->OnKeyDown(wParam);
		}
		else
		{
			// m_sample->OnKeyUp(wParam);
		}
#if 0
		//  (keypad enter doesn't have its own... VK_RETURN with KF_EXTENDED flag means keypad enter, see IM_VK_KEYPAD_ENTER definition for details, it is mapped to
		//  ImGuiKey_KeyPadEnter.)
		int vk = ((wParam == VK_RETURN) && (HIWORD(lParam) & KF_EXTENDED)) ? IM_VK_KEYPAD_ENTER : (int)wParam;

		auto get_key_down_state = [](int vk) -> bool
		{
			return (::GetKeyState(vk) & 0x8000) != 0;
		};

		auto convert_key = [](WPARAM wParam) -> ImGuiKey
		{
			static const std::unordered_map<WPARAM, ImGuiKey> conversion_map = {
				{VK_TAB, ImGuiKey_Tab},
				{VK_LEFT, ImGuiKey_LeftArrow},
				{VK_RIGHT, ImGuiKey_RightArrow},
				{VK_UP, ImGuiKey_UpArrow},
				{VK_DOWN, ImGuiKey_DownArrow},
				{VK_PRIOR, ImGuiKey_PageUp},
				{VK_NEXT, ImGuiKey_PageDown},
				{VK_HOME, ImGuiKey_Home},
				{VK_END, ImGuiKey_End},
				{VK_INSERT, ImGuiKey_Insert},
				{VK_DELETE, ImGuiKey_Delete},
				{VK_BACK, ImGuiKey_Backspace},
				{VK_SPACE, ImGuiKey_Space},
				{VK_RETURN, ImGuiKey_Enter},
				{VK_ESCAPE, ImGuiKey_Escape},
				{VK_OEM_7, ImGuiKey_Apostrophe},
				{VK_OEM_COMMA, ImGuiKey_Comma},
				{VK_OEM_MINUS, ImGuiKey_Minus},
				{VK_OEM_PERIOD, ImGuiKey_Period},
				{VK_OEM_2, ImGuiKey_Slash},
				{VK_OEM_1, ImGuiKey_Semicolon},
				{VK_OEM_PLUS, ImGuiKey_Equal},
				{VK_OEM_4, ImGuiKey_LeftBracket},
				{VK_OEM_5, ImGuiKey_Backslash},
				{VK_OEM_6, ImGuiKey_RightBracket},
				{VK_OEM_3, ImGuiKey_GraveAccent},
				{VK_CAPITAL, ImGuiKey_CapsLock},
				{VK_SCROLL, ImGuiKey_ScrollLock},
				{VK_NUMLOCK, ImGuiKey_NumLock},
				{VK_SNAPSHOT, ImGuiKey_PrintScreen},
				{VK_PAUSE, ImGuiKey_Pause},
				{VK_NUMPAD0, ImGuiKey_Keypad0},
				{VK_NUMPAD1, ImGuiKey_Keypad1},
				{VK_NUMPAD2, ImGuiKey_Keypad2},
				{VK_NUMPAD3, ImGuiKey_Keypad3},
				{VK_NUMPAD4, ImGuiKey_Keypad4},
				{VK_NUMPAD5, ImGuiKey_Keypad5},
				{VK_NUMPAD6, ImGuiKey_Keypad6},
				{VK_NUMPAD7, ImGuiKey_Keypad7},
				{VK_NUMPAD8, ImGuiKey_Keypad8},
				{VK_NUMPAD9, ImGuiKey_Keypad9},
				{VK_DECIMAL, ImGuiKey_KeypadDecimal},
				{VK_DIVIDE, ImGuiKey_KeypadDivide},
				{VK_MULTIPLY, ImGuiKey_KeypadMultiply},
				{VK_SUBTRACT, ImGuiKey_KeypadSubtract},
				{VK_ADD, ImGuiKey_KeypadAdd},
				{IM_VK_KEYPAD_ENTER, ImGuiKey_KeypadEnter},
				{VK_LSHIFT, ImGuiKey_LeftShift},
				{VK_LCONTROL, ImGuiKey_LeftCtrl},
				{VK_LMENU, ImGuiKey_LeftAlt},
				{VK_LWIN, ImGuiKey_LeftSuper},
				{VK_RSHIFT, ImGuiKey_RightShift},
				{VK_RCONTROL, ImGuiKey_RightCtrl},
				{VK_RMENU, ImGuiKey_RightAlt},
				{VK_RWIN, ImGuiKey_RightSuper},
				{VK_APPS, ImGuiKey_Menu},
				{'0', ImGuiKey_0},
				{'1', ImGuiKey_1},
				{'2', ImGuiKey_2},
				{'3', ImGuiKey_3},
				{'4', ImGuiKey_4},
				{'5', ImGuiKey_5},
				{'6', ImGuiKey_6},
				{'7', ImGuiKey_7},
				{'8', ImGuiKey_8},
				{'9', ImGuiKey_9},
				{'A', ImGuiKey_A},
				{'B', ImGuiKey_B},
				{'C', ImGuiKey_C},
				{'D', ImGuiKey_D},
				{'E', ImGuiKey_E},
				{'F', ImGuiKey_F},
				{'G', ImGuiKey_G},
				{'H', ImGuiKey_H},
				{'I', ImGuiKey_I},
				{'J', ImGuiKey_J},
				{'K', ImGuiKey_K},
				{'L', ImGuiKey_L},
				{'M', ImGuiKey_M},
				{'N', ImGuiKey_N},
				{'O', ImGuiKey_O},
				{'P', ImGuiKey_P},
				{'Q', ImGuiKey_Q},
				{'R', ImGuiKey_R},
				{'S', ImGuiKey_S},
				{'T', ImGuiKey_T},
				{'U', ImGuiKey_U},
				{'V', ImGuiKey_V},
				{'W', ImGuiKey_W},
				{'X', ImGuiKey_X},
				{'Y', ImGuiKey_Y},
				{'Z', ImGuiKey_Z},
				{VK_F1, ImGuiKey_F1},
				{VK_F2, ImGuiKey_F2},
				{VK_F3, ImGuiKey_F3},
				{VK_F4, ImGuiKey_F4},
				{VK_F5, ImGuiKey_F5},
				{VK_F6, ImGuiKey_F6},
				{VK_F7, ImGuiKey_F7},
				{VK_F8, ImGuiKey_F8},
				{VK_F9, ImGuiKey_F9},
				{VK_F10, ImGuiKey_F10},
				{VK_F11, ImGuiKey_F11},
				{VK_F12, ImGuiKey_F12}};

			if (auto itor = conversion_map.find(wParam); itor != conversion_map.end())
			{
				return itor->second;
			}
			else
			{
				return ImGuiKey_None;
			}
		};

		if (wParam < 256)
		{
			// Submit modifiers
			ImGuiIO& io = ImGui::GetIO();
			io.AddKeyEvent(ImGuiKey_ModCtrl, get_key_down_state(VK_CONTROL));
			io.AddKeyEvent(ImGuiKey_ModShift, get_key_down_state(VK_SHIFT));
			io.AddKeyEvent(ImGuiKey_ModAlt, get_key_down_state(VK_MENU));
			io.AddKeyEvent(ImGuiKey_ModSuper, get_key_down_state(VK_APPS));

			auto add_key_event = [&io](ImGuiKey key, bool down, int native_keycode, int native_scancode = -1)
			{
				io.AddKeyEvent(key, down);
				io.SetKeyEventNativeData(key, native_keycode, native_scancode); // To support legacy indexing (<1.87 user code)
				IM_UNUSED(native_scancode);
			};

			// Submit key event
			const ImGuiKey key		= convert_key(vk);
			const int	   scancode = (int)LOBYTE(HIWORD(lParam));
			if (key != ImGuiKey_None)
			{
				add_key_event(key, is_key_down, vk, scancode);
			}

			// Submit individual left/right modifier events
			if (vk == VK_SHIFT)
			{
				// Important: Shift keys tend to get stuck when pressed together, missing key-up events are corrected in ImGui_ImplWin32_ProcessKeyEventsWorkarounds()
				if (get_key_down_state(VK_LSHIFT) == is_key_down)
				{
					add_key_event(ImGuiKey_LeftShift, is_key_down, VK_LSHIFT, scancode);
				}
				if (get_key_down_state(VK_RSHIFT) == is_key_down)
				{
					add_key_event(ImGuiKey_RightShift, is_key_down, VK_RSHIFT, scancode);
				}
			}
			else if (vk == VK_CONTROL)
			{
				if (get_key_down_state(VK_LCONTROL) == is_key_down)
				{
					add_key_event(ImGuiKey_LeftCtrl, is_key_down, VK_LCONTROL, scancode);
				}
				if (get_key_down_state(VK_RCONTROL) == is_key_down)
				{
					add_key_event(ImGuiKey_RightCtrl, is_key_down, VK_RCONTROL, scancode);
				}
			}
			else if (vk == VK_MENU)
			{
				if (get_key_down_state(VK_LMENU) == is_key_down)
				{
					add_key_event(ImGuiKey_LeftAlt, is_key_down, VK_LMENU, scancode);
				}
				if (get_key_down_state(VK_RMENU) == is_key_down)
				{
					add_key_event(ImGuiKey_RightAlt, is_key_down, VK_RMENU, scancode);
				}
			}
		}
#endif
	}

	void configure_input_messages()
	{
		on_message(
			WM_MOUSEMOVE,
			[&](wl::wm::mousemove p) -> LRESULT
			{
				m_mouse_hwnd = hwnd();

				if (!m_mouse_tracked)
				{
					TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hwnd(), 0};
					::TrackMouseEvent(&tme);
					m_mouse_tracked = true;
				}

				// ImGui::GetIO().AddMousePosEvent(p.pos().x, p.pos().y);

				return 0;
			});

		on_message(
			WM_MOUSELEAVE,
			[&](wl::wm::mouseleave p) -> LRESULT
			{
				if (m_mouse_hwnd == hwnd())
				{
					m_mouse_hwnd = NULL;
				}

				m_mouse_tracked = false;

				// ImGui::GetIO().AddMousePosEvent(-FLT_MAX, -FLT_MAX);

				return 0;
			});

		on_message(
			WM_LBUTTONDOWN,
			[&](wl::wm::lbuttondown p) -> LRESULT
			{
				mouse_button_down<WM_LBUTTONDOWN>(p.wParam);
				return 0;
			});

		on_message(
			WM_RBUTTONDOWN,
			[&](wl::wm::rbuttondown p) -> LRESULT
			{
				mouse_button_down<WM_RBUTTONDOWN>(p.wParam);
				return 0;
			});

		on_message(
			WM_MBUTTONDOWN,
			[&](wl::wm::mbuttondown p) -> LRESULT
			{
				mouse_button_down<WM_MBUTTONDOWN>(p.wParam);
				return 0;
			});

		on_message(
			WM_LBUTTONUP,
			[&](wl::wm::lbuttonup p) -> LRESULT
			{
				mouse_button_up<WM_LBUTTONUP>(p.wParam);
				return 0;
			});

		on_message(
			WM_RBUTTONUP,
			[&](wl::wm::rbuttonup p) -> LRESULT
			{
				mouse_button_up<WM_RBUTTONUP>(p.wParam);
				return 0;
			});

		on_message(
			WM_RBUTTONUP,
			[&](wl::wm::mbuttonup p) -> LRESULT
			{
				mouse_button_up<WM_RBUTTONUP>(p.wParam);
				return 0;
			});

		on_message(
			WM_MOUSEWHEEL,
			[&](wl::wm::mousewheel p) -> LRESULT
			{
				// ImGui::GetIO().AddMouseWheelEvent(0.0f, (float)p.wheel_delta() / (float)WHEEL_DELTA);
				return 0;
			});

		on_message(
			WM_MOUSEHWHEEL,
			[&](wl::wm::mousewheel p) -> LRESULT
			{
				// ImGui::GetIO().AddMouseWheelEvent((float)p.wheel_delta() / (float)WHEEL_DELTA, 0.0f);
				return 0;
			});

		on_message(
			WM_SETFOCUS,
			[&](wl::wm::setfocus p) -> LRESULT
			{
				// ImGui::GetIO().AddFocusEvent(true);
				return 0;
			});

		on_message(
			WM_KILLFOCUS,
			[&](wl::wm::setfocus p) -> LRESULT
			{
				// ImGui::GetIO().AddFocusEvent(false);
				return 0;
			});

		// on_message(
		//	WM_SETCURSOR,
		//	[&](wl::wm::setcursor p) -> LRESULT
		//	{
		//		if (LOWORD(p.lParam) == HTCLIENT && update_mouse_cursor())
		//		{
		//			return 1;
		//		}
		//		return 0;
		//	});

		on_message(
			WM_KEYDOWN,
			[&](wl::wm::keydown p) -> LRESULT
			{
				handle_key<true>(p.wParam, p.lParam);
				return 0;
			});

		on_message(
			WM_SYSKEYDOWN,
			[&](wl::wm::syskeydown p) -> LRESULT
			{
				handle_key<true>(p.wParam, p.lParam);
				return 0;
			});

		on_message(
			WM_KEYUP,
			[&](wl::wm::keyup p) -> LRESULT
			{
				handle_key<false>(p.wParam, p.lParam);
				return 0;
			});

		on_message(
			WM_SYSKEYUP,
			[&](wl::wm::syskeyup p) -> LRESULT
			{
				handle_key<false>(p.wParam, p.lParam);
				return 0;
			});

		on_message(
			WM_CHAR,
			[&](wl::wm::menuchar p) -> LRESULT
			{
				// if (p.wParam > 0 && p.wParam < 0x10000)
				//{
				//	ImGui::GetIO().AddInputCharacterUTF16((unsigned short)p.wParam);
				// }

				return 0;
			});
	}

	vvv::result<void> vulkan_init(int argc, char** argv)
	{
		static std::shared_ptr<vvv::vk_application> s_vk_application;

		if (!m_vk_instance)
		{
			if (!s_vk_application)
			{
				vvv_LEAF_ASSIGN(
					s_vk_application,
					vvv::create_application(
						[&](auto& app_info) -> vvv::result<void>
						{
							app_info->with_defaults().with_cmdline(argc, argv);
							return {};
						}));
			}

			vvv_LEAF_ASSIGN(
				m_vk_instance,
				s_vk_application->create_instance(
					[&](auto& instance_ci) -> vvv::result<void>
					{
						instance_ci->with_default_extensions().with_default_layers();
						return {};
					}));
		}

		return {};
	}

	vvv::result<void> vulkan_frame_init()
	{
		if (!m_vk_physical_device)
		{
			vvv_LEAF_AUTO(physical_devices, m_vk_instance->discover_suitable_physical_devices());

			vvv_LEAF_ASSIGN(
				m_vk_physical_device,
				m_vk_instance->create_physical_device(
					[&](auto& phys_dev_ci) -> vvv::result<void>
					{
						phys_dev_ci->with_device_info(physical_devices[0]);
						return {};
					}));

			vvv_LEAF_ASSIGN(m_vk_queue_families, m_vk_physical_device->create_queue_families());

			vvv_LEAF_ASSIGN(m_vk_surface, m_vk_instance->create_surface(hwnd()));

			vvv_LEAF_ASSIGN(
				m_vk_device,
				m_vk_physical_device->create_device(
					[&](auto& device_create_info) -> vvv::result<void>
					{
						vvv_LEAF_AUTO(queue_families, m_vk_queue_families->find_exclusive_queue_families(/*m_vk_surface->get_vk_surface()*/));

						device_create_info->with_default_queue_families(queue_families);
						device_create_info->with_default_extensions();
						return {};
					}));
		}

		if (!m_vk_swapchain)
		{
			vvv_LEAF_AUTO(surf_caps, m_vk_surface->get_surface_capabilities(*m_vk_physical_device));

			vvv_LEAF_ASSIGN(
				m_vk_swapchain,
				m_vk_device->create_swapchain(
					[&](auto& create_info) -> vvv::result<void>
					{
						create_info
							->with_settings(
								std::clamp(uint32_t(2), surf_caps.m_capabilities.minImageCount, surf_caps.m_capabilities.maxImageCount),
								VK_FORMAT_B8G8R8A8_SRGB,
								VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
								surf_caps.m_capabilities.currentTransform,
								VK_PRESENT_MODE_MAILBOX_KHR,
								VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
							.with_size(
								{std::clamp(uint32_t(m_rect.right - m_rect.left), surf_caps.m_capabilities.minImageExtent.width, surf_caps.m_capabilities.maxImageExtent.width),
								 std::clamp(uint32_t(m_rect.bottom - m_rect.top), surf_caps.m_capabilities.minImageExtent.height, surf_caps.m_capabilities.maxImageExtent.height)})
							.with_surface(m_vk_surface);
						return {};
					}));
		}

		return {};
	}

#if 0
	vvv::result<void> vulkan_init_swapchain()
	{
		vvv_LEAF_AUTO(surf_caps, m_vk_surface->get_surface_capabilities(*m_vk_physical_device));

		auto create_info = vvv::vk_swapchain_create_info::factory(m_vk_surface);


		vvv_LEAF_ASSIGN(m_vk_swapchain, m_vk_device->create_swapchain(create_info));
		return {};
	}
#endif

	vvv::result<void> vulkan_destroy()
	{
		return {};
	}

	root_window(int argc, char** argv)
	{
		if (auto res = vulkan_init(argc, argv); !res) [[unlikely]]
		{
			vvv::leaf::throw_exception(res.get_error_id());
		}

		::QueryPerformanceFrequency((LARGE_INTEGER*)&m_ticks_per_second);
		::QueryPerformanceCounter((LARGE_INTEGER*)&m_last_time);

		setup.wndClassEx.style		   = CS_CLASSDC;
		setup.wndClassEx.lpszClassName = L"ImGui Example"; // class name to be registered
		setup.title					   = L"This is my window";
		setup.style |= WS_MINIMIZEBOX | WS_OVERLAPPEDWINDOW;

		configure_input_messages();

		on_message(
			WM_CREATE,
			[&](wl::wm::create p) -> LRESULT
			{
				return 0;
			});

		on_message(
			WM_SIZE,
			[&](wl::wm::size p) -> LRESULT
			{
				return 0;
			});

		on_message(
			WM_SIZING,
			[&](wl::wm::size p) -> LRESULT
			{
				return 0;
			});

		on_message(
			renderloop_message(),
			[&](wl::wm::command p) -> LRESULT
			{
				::GetClientRect(hwnd(), &m_rect);

				if (auto res = vulkan_frame_init(); !res) [[unlikely]]
				{
					vvv::leaf::throw_exception(res.get_error_id());
				}
#if 0
				if (!m_vk_device)
				{
					if (auto res = vulkan_init_device(); !res)
					{
						vvv::leaf::throw_exception(res.get_error_id());
					}
				}

				if (!m_vk_surface)
				{
					if (auto res = vulkan_init_surface(); !res)
					{
						vvv::leaf::throw_exception(res.get_error_id());
					}
				}

				if (!m_vk_swapchain)
				{
					if (auto res = vulkan_init_swapchain(); !res)
					{
						vvv::leaf::throw_exception(res.get_error_id());
					}
				}
#endif
				// if (!m_sample)
				//{
				//	m_sample = std::make_unique<T_EXAMPLE>((m_rect.right - m_rect.left), (m_rect.bottom - m_rect.top));
				//	m_sample->OnInit(hwnd());
				// }

				update_mouse_data();

				INT64 current_time = 0;
				::QueryPerformanceCounter((LARGE_INTEGER*)&current_time);
				::QueryPerformanceFrequency((LARGE_INTEGER*)&m_ticks_per_second);
				m_delta_time = ((double)(current_time - m_last_time) / (double)m_ticks_per_second);
				m_last_time	 = current_time;

				// if (m_sample)
				//{
				//	m_sample->OnUpdate();
				//	m_sample->OnRender();
				// }

				// io.DisplaySize = ImVec2((float)(m_rect.right - m_rect.left), (float)(m_rect.bottom - m_rect.top));
				// io.DeltaTime = (float)m_delta_time;

				// Update OS mouse cursor with the cursor requested by imgui
				// ImGuiMouseCursor mouse_cursor = io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();
				// if (m_last_mouse_cursor != mouse_cursor)
				//{
				//	m_last_mouse_cursor = mouse_cursor;
				//	update_mouse_cursor();
				//}

				return 0;
			});
	}

	~root_window()
	{
		// if (m_sample)
		//{
		//	m_sample->OnDestroy();
		// }

		vulkan_destroy();
	}
};
