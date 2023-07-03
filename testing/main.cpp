#include <catch2/catch_test_macros.hpp>

#include <vvv_window.h>

int main(int argc, char** argv)
{
	vvv::init(argc, argv);

	return vvv::leaf::try_handle_all(
		[&]() -> vvv::leaf::result<int>
		{
			return wl::_wli::leaky_run_main<root_window>(::GetModuleHandleA(NULL), SW_SHOWDEFAULT);
		},
		[](vvv::runtime_error::not_specified err)
		{
			// TODO: spdlog::error(err.m_description);
			return -1;
		},
		[](vvv::leaf::error_info const& unmatched)
		{
			return -1;
		});
}
