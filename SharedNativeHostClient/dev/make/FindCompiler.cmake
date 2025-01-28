set(CLANG_VERSION 18)

# -----------------------------------------------------------------------------
# Configures cmake to build with msvc
# -----------------------------------------------------------------------------
function(configure_windows_compiler)
    find_program(CL cl.exe REQUIRED)
    set(CMAKE_CXX_COMPILER "${CL}" PARENT_SCOPE)
endfunction()

# -----------------------------------------------------------------------------
# Configures cmake to build with LLVM clang
# -----------------------------------------------------------------------------
function(configure_macos_compiler)
    set(llvm_formula_name llvm@${CLANG_VERSION})

    execute_process(
        COMMAND brew --prefix ${llvm_formula_name}
        RESULT_VARIABLE result
        OUTPUT_VARIABLE llvm_path
    )

    if (NOT(${result} EQUAL 0))
        message(FATAL_ERROR "Failed to get the ${llvm_formula_name} formula path. brew exit code: ${result}. brew output: ${llvm_path}")
    endif()

    string(STRIP "${llvm_path}" llvm_path)

    # Specify the compilers to use on MacOS
    set(CMAKE_C_COMPILER "${llvm_path}/bin/clang" PARENT_SCOPE)
    set(CMAKE_CXX_COMPILER "${llvm_path}/bin/clang++" PARENT_SCOPE)
    set(CMAKE_OBJC_COMPILER "${llvm_path}/bin/clang" PARENT_SCOPE)
    set(CMAKE_OBJCXX_COMPILER "${llvm_path}/bin/clang++" PARENT_SCOPE)

    # CMAKE_SYSTEM_NAME should always be "Darwin" on Mac, but without this check, we can
    # hork vcpkg when this is included in the chainload toolchain file.
    # Working theory is that this gets evaluated early, before CMAKE_SYSTEM_NAME is set,
    # and add_link_options sticks but link_directories doesn't, resulting in linker errors
    # because 'unwind' library can't be found
    if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
        # Make sure the C++ standard library files we link against are from the llvm we installed,
        # not the default ones installed with XCode
        link_directories(BEFORE "${llvm_path}/lib" "${llvm_path}/lib/c++")

        # Link against unwind library, which is necessary for stack traces
        add_link_options(LINKER:-lunwind)

        # cmake can add libraries multiple times in link commands. It adds direct dependencies and then appends transitive dependencies.
        # linking on mac produces 'ignoring duplicate library' warnings, which we don't, so suppress it
        add_link_options(LINKER:-no_warn_duplicate_libraries)
    endif()

endfunction()

# -----------------------------------------------------------------------------
# Configures cmake to build with LLVM clang
# -----------------------------------------------------------------------------
function(configure_linux_compiler)
    read_clang_version(llvm_version)
    find_program(CLANGXX clang++-18 REQUIRED)
    set(CMAKE_CXX_COMPILER "${CLANGXX}" PARENT_SCOPE)
endfunction()

# -----------------------------------------------------------------------------
# Configures compiler if not already set
# -----------------------------------------------------------------------------
if ("${CMAKE_CXX_COMPILER}" STREQUAL "")
    # compiler isn't already set up
    if (PLATFORM_WIN)
        configure_windows_compiler()
    elseif (PLATFORM_MAC)
        configure_macos_compiler()
    elseif (PLATFORM_LINUX)
        configure_linux_compiler()
    else()
        message(FATAL_ERROR "Unsupported system type")
    endif()
endif()
