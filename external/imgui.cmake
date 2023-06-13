CPMAddPackage(
  NAME imgui
  GITHUB_REPOSITORY "ocornut/imgui"
  GIT_TAG 7348e99269c93b975dac3a4f4c68e58ebce078df
  DOWNLOAD_ONLY YES)

set(imgui_SOURCE_FILES
    "${imgui_SOURCE_DIR}/imconfig.h"
    "${imgui_SOURCE_DIR}/imgui.cpp"
    "${imgui_SOURCE_DIR}/imgui.h"
    "${imgui_SOURCE_DIR}/imgui_internal.h"
	"${imgui_SOURCE_DIR}/imgui_demo.cpp"
    "${imgui_SOURCE_DIR}/imgui_draw.cpp"
    "${imgui_SOURCE_DIR}/imgui_tables.cpp"
    "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
    "${imgui_SOURCE_DIR}/imstb_rectpack.h"
    "${imgui_SOURCE_DIR}/imstb_textedit.h"
    "${imgui_SOURCE_DIR}/imstb_truetype.h")

add_library(imgui ${imgui_SOURCE_FILES})

target_include_directories(imgui PUBLIC
    "$<BUILD_INTERFACE:${imgui_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

add_library(imgui::imgui ALIAS imgui)