target_sources (${PROJECT_NAME}
  PRIVATE
    "./fi.c"
    "./buffer.c"
)

if (WIN32)
  add_subdirectory ("./windows")
elseif (UNIX)
  add_subdirectory ("./posix")
endif ()
