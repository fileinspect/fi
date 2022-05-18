function (CCompileOptions target scope warnings_as_errors)
  if (MSVC)
    target_compile_options (${target}
      ${scope}
        /Wall
        /wd4710
        /wd4711
        /wd5045
        /wd4820
        /wd4706
        /wd4255
        /wd4090
        /Qspectre
    )
    target_compile_definitions (${target}
      ${scope}
        -D_CRT_SECURE_NO_WARNINGS
    )
    if (${warnings_as_errors})
      target_compile_options (${target}
        ${scope}
          /WX
      )
    endif ()
  else ()
    target_compile_options (${target}
      ${scope}
        -Wall
        -Wextra
        -pedantic
    )
    if (${warnings_as_errors})
      target_compile_options (${target}
        ${scope}
          -Werror
      )
    endif ()
  endif ()
endfunction ()
