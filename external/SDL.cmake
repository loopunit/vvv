set(SDL_PATCHFILE ${CMAKE_CURRENT_LIST_DIR}/SDL.patch)
set(SDL_PATCH_COMMAND git apply ${SDL_PATCHFILE})

if (EXISTS ${SDL_PATCHFILE})
	set(SDL_EXTRA_ARGS PATCH_COMMAND ${SDL_PATCH_COMMAND})
endif()

CPMAddPackage(
  NAME SDL
  GITHUB_REPOSITORY "libsdl-org/SDL"
  GIT_TAG 3ddbeab88fcf5bc1b09fb664403b1240d342a6ae
  ${SDL_EXTRA_ARGS}
  OPTIONS
	"SDL_DISABLE_INSTALL OFF"
	"SDL_DISABLE_INSTALL_CPACK OFF"
    "SDL_TEST OFF"
    "SDL_RENDER_D3D OFF"
    "SDL_OPENGL OFF"
    "SDL_OPENGLES OFF"
    "SDL_DIRECTX OFF"
    "SDL_VULKAN OFF"
	"BUILD_SHARED_LIBS OFF")

#message(STATUS "Generate the patch thusly:")
#message(STATUS "cd ${SDL_SOURCE_DIR}")
#message(STATUS "git diff > ${SDL_PATCHFILE}")
