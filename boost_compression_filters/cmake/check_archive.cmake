if(NOT EXISTS "${ARCHIVE}")
    message(FATAL_ERROR "Compression archive does not exist: ${ARCHIVE}")
endif()
execute_process(COMMAND "${NM}" --defined-only --format=posix "${ARCHIVE}"
    RESULT_VARIABLE result OUTPUT_VARIABLE symbols ERROR_VARIABLE errors)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "Cannot inspect ${ARCHIVE}: ${errors}")
endif()
if(symbols MATCHES "(^|\n)(main|wmain|WinMain|wWinMain|_main|_wmain|_WinMain@16|_wWinMain@16)[ \t]+")
    message(FATAL_ERROR
        "${ARCHIVE} defines a program entry point. Exclude test/utility sources and rebuild the archive.")
endif()
message(STATUS "Verified archive has no program entry points: ${ARCHIVE}")
