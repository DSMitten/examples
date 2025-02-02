# Copyright (C) Microsoft Corporation. All rights reserved.

if (WIN32)
    set(PLATFORM_WIN TRUE)
elseif (UNIX)
    execute_process(COMMAND uname RESULT_VARIABLE uname_result OUTPUT_VARIABLE uname_output)
    if (NOT ${uname_result} EQUAL 0)
        message(FATAL_ERROR "uname failed. exit code: ${uname_result}. Output: ${uname_output}")
    endif()

    string(STRIP "${uname_output}" uname_output)

    if ("${uname_output}" STREQUAL "Darwin")
        set(PLATFORM_MAC TRUE)
        set(PLATFORM_POSIX TRUE)
    elseif("${uname_output}" STREQUAL "Linux")
        set(PLATFORM_LINUX TRUE)
        set(PLATFORM_POSIX TRUE)
    else()
        message(FATAL_ERROR "Unsupported UNIX platform: '${uname_output}'")
    endif()
else()
    message(FATAL_ERROR "Unsupported system type")
endif()
