set(boost_PATCHFILE ${CMAKE_CURRENT_LIST_DIR}/boost.patch)
set(boost_PATCH_COMMAND git apply ${boost_PATCHFILE})

if (EXISTS ${boost_PATCHFILE})
	set(boost_EXTRA_ARGS PATCH_COMMAND ${boost_PATCH_COMMAND})
endif()

CPMAddPackage(
  NAME boost
  GITHUB_REPOSITORY "boostorg/boost"
  GIT_TAG cebb17891aecfb2c30285c60c80740a79c5d0c51
  ${boost_EXTRA_ARGS}
  OPTIONS
    "BOOST_ENABLE_MPI OFF"
    "BOOST_ENABLE_PYTHON ON"
	"BOOST_INSTALL_LAYOUT system"
	"BOOST_SKIP_INSTALL_RULES OFF"
	"BUILD_SHARED_LIBS OFF")

#message(STATUS "Generate the patch thusly:")
#message(STATUS "cd ${boost_SOURCE_DIR}")
#message(STATUS "git diff > ${boost_PATCHFILE}")
