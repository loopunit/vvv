CPMAddPackage(
  NAME SPIRV-Reflect
  GITHUB_REPOSITORY "KhronosGroup/SPIRV-Reflect"
  GIT_TAG 1fd43331f0bd77cc0f421745781f79a14d8f2bb1
  OPTIONS
    "SPIRV_SKIP_TESTS ON"
    "SPIRV_REFLECT_EXAMPLES OFF"
    "SPIRV_CROSS_ENABLE_TESTS OFF")
  #DOWNLOAD_ONLY YES)

