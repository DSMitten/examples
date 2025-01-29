
# Use C++20 language standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

# Automatically add the current source and build directories to the current target's include path.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Treat warnings as errors
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

# On Linux and Mac, compile position independent code and build position independent executables.
if (PLATFORM_POSIX)
    # Note we have to call check_pie_supported for CMAKE_POSITION_INDEPENDENT_CODE to set compiler and linker flags correctly
    include(CheckPIESupported)
    check_pie_supported(LANGUAGES CXX)

    set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
endif()

# Don't export symbols by default from shared libraries
# (Not needed for Windows because dll symbols are not visible by default)
if (PLATFORM_POSIX)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_OBJCXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)
endif()

if (PLATFORM_MAC)
    # Enable setting macos rpath
    # Useful reference: https://www.mikeash.com/pyblog/friday-qa-2009-11-06-linking-and-install-names.html
    set(CMAKE_MACOSX_RPATH TRUE)
else()
    # Set shared library RUNPATH to $ORIGIN because our shared libraries will be located next to executables
    # Useful reference: https://nehckl0.medium.com/creating-relocatable-linux-executables-by-setting-rpath-with-origin-45de573a2e98
    list(APPEND CMAKE_INSTALL_RPATH "$ORIGIN")
endif()

if (PLATFORM_WIN)
    # Add definitions when compiling source files
    add_compile_definitions(
        UNICODE            # Specify unicode for Windows header files
        NOMINMAX           # When including <Windows.h>, don't define 'min' or 'max' macros
    )
endif()

# wv2-host package headers look for PLATFORM_ macros for conditional platform-specific code
if (PLATFORM_WIN)
    add_compile_definitions(PLATFORM_WIN)
elseif (PLATFORM_MAC)
    add_compile_definitions(PLATFORM_MAC)
elseif (PLATFORM_LINUX)
    add_compile_definitions(PLATFORM_LINUX)
else()
    message(FATAL_ERROR "No supported platform was selected.")
endif()

if (PLATFORM_POSIX)
    add_compile_definitions(PLATFORM_POSIX)
endif()

if(PROCESSOR_X64)
    add_compile_definitions(PROCESSOR_X64)
elseif(PROCESSOR_X86)
    add_compile_definitions(PROCESSOR_X86)
elseif(PROCESSOR_ARM64)
    add_compile_definitions(PROCESSOR_ARM64)
else()
    message(FATAL_ERROR "No supported processor architecture was selected.")
endif()

if (MSVC)
    # If the caller set a warning level, remove it because we set it ourselves
    string(REPLACE "/W3" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

    # Set warning level 4
    add_compile_options(/W4)

    if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        # CMake might set these to on when building with VS and it will interfere with incremental linking
        add_link_options(
            /OPT:NOREF
            /OPT:NOICF
            /OPT:NOLBR
        )
    else()
        # If we're optimizing, don't try to link incrementally
        add_link_options(
            /OPT:REF
            /OPT:ICF
            /OPT:LBR
            /INCREMENTAL:NO
        )
    endif()

    # Add definitions when compiling source files
    add_compile_definitions(
        _UNICODE                        # Specify unicode for CRT header files
    )
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
    if (PLATFORM_MAC)
        # Enable automatic reference counting for objc types
        add_compile_options(-fobjc-arc)

        # work around https://github.com/llvm/llvm-project/issues/77773 [libc++] does not define __cpp_lib_format
        # The LLVM format implementation is incomplete so doesn't define this macro, but it is good enough for most needs so define it
        # libc++19 will define this so it won't need to be defined once that version is adopted
        add_compile_definitions(__cpp_lib_format=202106L)
    elseif (PLATFORM_LINUX)
        # Ensure that the threading library is linked
        add_compile_options(-pthread)
        add_link_options(-pthread)
    endif()
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL AppleClang)
    message(FATAL_ERROR "AppleClang compiler is not supported - use LLVM Clang to build for MacOS")
else()
    message(FATAL_ERROR "Unsupported compiler ${CMAKE_CXX_COMPILER_ID}")
endif()
