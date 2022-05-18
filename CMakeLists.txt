cmake_minimum_required (VERSION 3.13)

set (PROJECT_NAME "fi_core")

project (${PROJECT_NAME}
  VERSION 0.0.1
  DESCRIPTION "Modern modal text editing."
  HOMEPAGE_URL "https://github.com/fileinspector/fi"
  LANGUAGES C
)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. \
      Please make a new directory (called a build directory) \
      and run CMake from there."
  )
endif()

set (CMAKE_C_STANDARD 11)
set (CMAKE_C_STANDARD_REQUIRED ON)

include (cmake/CCompileOptions.cmake)
include (cmake/Libraries.cmake)

add_executable (${PROJECT_NAME})
CCompileOptions(${PROJECT_NAME} PRIVATE ON)
target_include_directories (${PROJECT_NAME}
  PRIVATE
    "./include"
)
target_link_libraries (${PROJECT_NAME} uv_a)
add_subdirectory ("./src")

install (TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

if (BUILD_TESTING)
  add_subdirectory ("./test")
endif (BUILD_TESTING)
