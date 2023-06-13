CPMAddPackage(
  NAME imgui-docking
  GITHUB_REPOSITORY "ocornut/imgui"
  GIT_TAG 530134d1253c97bd33268b5932b6c67d0ad47d09
  DOWNLOAD_ONLY YES)

set(imgui-docking_SOURCE_FILES
    "${imgui-docking_SOURCE_DIR}/imconfig.h"
    "${imgui-docking_SOURCE_DIR}/imgui.cpp"
    "${imgui-docking_SOURCE_DIR}/imgui.h"
    "${imgui-docking_SOURCE_DIR}/imgui_internal.h"
    "${imgui-docking_SOURCE_DIR}/imgui_rectpack.h"
    "${imgui-docking_SOURCE_DIR}/imgui_textedit.h"
    "${imgui-docking_SOURCE_DIR}/imgui_truetype.h"
	"${imgui-docking_SOURCE_DIR}/imgui_demo.cpp"
    "${imgui-docking_SOURCE_DIR}/imgui_draw.cpp"
    "${imgui-docking_SOURCE_DIR}/imgui_tables.cpp"
    "${imgui-docking_SOURCE_DIR}/imgui_widgets.cpp")

add_library(imgui-docking ${imgui-docking_SOURCE_FILES})

target_include_directories(imgui-docking PUBLIC
    "$<BUILD_INTERFACE:${imgui-docking_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

add_library(imgui::imgui-docking ALIAS imgui-docking)