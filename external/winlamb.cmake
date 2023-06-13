CPMAddPackage(
  NAME winlamb
  GITHUB_REPOSITORY "rodrigocfd/winlamb"
  GIT_TAG 85b932efff51c52bc772830382ae551a0ccda2dc
  DOWNLOAD_ONLY YES)

add_library(winlamb INTERFACE)
target_include_directories(winlamb INTERFACE 
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	$<BUILD_INTERFACE:${winlamb_SOURCE_DIR}>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/ext>)

add_library(winlamb::winlamb ALIAS winlamb)