# Copyright (C) Microsoft Corporation. All rights reserved.

# The repo uses vpckg for package management, and sets VCPKG_TARGET_TRIPLET in CMakePresets.json
function (get_processor_arch output_variable)
    if (DEFINED VCPKG_TARGET_TRIPLET)
        string(REGEX MATCH "^[^-]+" processor ${VCPKG_TARGET_TRIPLET})
        if (NOT DEFINED processor)
            message(FATAL_ERROR "Could not detect processor architecture from VCPKG_TARGET_TRIPLET: ${VCPKG_TARGET_TRIPLET}")
        endif()
    elseif (DEFINED VCPKG_TARGET_ARCHITECTURE)
        set(processor ${VCPKG_TARGET_ARCHITECTURE})
    else()
        message(FATAL_ERROR "Cannot determine target processor architecture as neither VCPKG_TARGET_TRIPLET nor VCPKG_TARGET_ARCHITECTURE are defined")
    endif()

    set(${output_variable} ${processor} PARENT_SCOPE)
endfunction()


# Detects the platform being built on and sets the appropriate variables
function (set_config_variables)
    # PLATFORM_WIN, PLATFORM_MAC, PLATFORM_LINUX, PLATFORM_POSIX
    if (WIN32)
        set(PLATFORM_WIN TRUE PARENT_SCOPE)
    elseif (UNIX)
        execute_process(COMMAND uname RESULT_VARIABLE uname_result OUTPUT_VARIABLE uname_output)
        if (NOT ${uname_result} EQUAL 0)
            message(FATAL_ERROR "uname failed. exit code: ${uname_result}. Output: ${uname_output}")
        endif()

        string(STRIP "${uname_output}" uname_output)

        if ("${uname_output}" STREQUAL "Darwin")
            set(PLATFORM_MAC TRUE PARENT_SCOPE)
            set(PLATFORM_POSIX TRUE PARENT_SCOPE)
        elseif("${uname_output}" STREQUAL "Linux")
            set(PLATFORM_LINUX TRUE PARENT_SCOPE)
            set(PLATFORM_POSIX TRUE PARENT_SCOPE)
        else()
            message(FATAL_ERROR "Unsupported UNIX platform: '${uname_output}'")
        endif()
    else()
        message(FATAL_ERROR "Unsupported system type")
    endif()

    # PROCESSOR_X64, PROCESSOR_X86, PROCESSOR_ARM64
    get_processor_arch(processor_arch)
    if ("${processor_arch}" STREQUAL "x64")
        set(PROCESSOR_X64 TRUE PARENT_SCOPE)
    elseif ("${processor_arch}" STREQUAL "x86")
        set(PROCESSOR_X86 TRUE PARENT_SCOPE)
    elseif ("${processor_arch}" STREQUAL "arm64")
        set(PROCESSOR_ARM64 TRUE PARENT_SCOPE)
    else()
        message(FATAL_ERROR "Unsupported processor architecture: '${processor_arch}'")
    endif()
endfunction()

set_config_variables()
