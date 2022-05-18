cmake_minimum_required (VERSION 3.11)
include (FetchContent)

# Declarations
FetchContent_Declare (
  libuv
  GIT_REPOSITORY https://github.com/libuv/libuv.git
  GIT_TAG v1.44.1
)


# Populate
if (NOT libuv_POPULATED)
  FetchContent_Populate (libuv)
endif ()
set (BUILD_TESTING OFF)
add_subdirectory (${libuv_SOURCE_DIR})
