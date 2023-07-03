set(volk_PATCHFILE ${CMAKE_CURRENT_LIST_DIR}/volk.patch)
set(volk_PATCH_COMMAND git apply ${volk_PATCHFILE})

if (EXISTS ${volk_PATCHFILE})
	set(volk_EXTRA_ARGS PATCH_COMMAND ${volk_PATCH_COMMAND})
endif()

CPMAddPackage(
  NAME volk
  GITHUB_REPOSITORY "zeux/volk"
  GIT_TAG 6a585a41e8ec760f395d106e88423dace15cc206
  ${volk_EXTRA_ARGS}
  OPTIONS
	"VOLK_PULL_IN_VULKAN ON"
	"VOLK_INSTALL ON")
  #DOWNLOAD_ONLY YES)

#message(STATUS "Generate the patch thusly:")
#message(STATUS "cd ${volk_SOURCE_DIR}")
#message(STATUS "git diff > ${volk_PATCHFILE}")
