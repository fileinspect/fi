target_sources (${PROJECT_NAME}
  PRIVATE
    "./fi.c"
)

if (WIN32)
  add_subdirectory ("./windows")
elseif (UNIX)
  add_subdirectory ("./posix")
endif ()
