# Sets environment variables for the compiler based on the platform

# Windows: runs vsdevcmd.bat to set environment variables
# MacOS: no environment variables needed
# Linux: no environment variables needed

# -----------------------------------------------------------------------------
# Runs vswhere.exe to get a variable
# -----------------------------------------------------------------------------
function (get_visual_studio_variable variable_name output_variable)

    set(vswhere_name "vswhere.exe")
    set(vswhere_path "$ENV{ProgramFiles\(x86\)}/Microsoft Visual Studio/Installer/${vswhere_name}")
    cmake_path(NORMAL_PATH vswhere_path)

    if (NOT EXISTS "${vswhere_path}")
        message(FATAL_ERROR "Could not find ${vswhere_name}")
    endif()

    execute_process(
        COMMAND "${vswhere_path}" -latest -property ${variable_name}
        RESULT_VARIABLE result
        OUTPUT_VARIABLE vswhere_output
    )
    
    if (NOT(${result} EQUAL 0))
        message(STATUS "${vswhere_output}")
        message(FATAL_ERROR "Failed to get the visual studio path: ${result}")
    endif()

    string(STRIP "${vswhere_output}" vswhere_output)
    set(${output_variable} "${vswhere_output}" PARENT_SCOPE)

endfunction()

# -----------------------------------------------------------------------------
# Gets visual studio installation information
# -----------------------------------------------------------------------------
function (find_visual_studio_info vs_install_path_variable vs_install_version_variable)

    get_visual_studio_variable(installationPath variable_value)
    set(${vs_install_path_variable} "${variable_value}" PARENT_SCOPE)

    get_visual_studio_variable(installationVersion variable_value)
    set(${vs_install_version_variable} "${variable_value}" PARENT_SCOPE)

endfunction()

function (get_msvc_environment_variables_cache_path target_arch output_variable)
    set(vsdevcmd_cache_json_path ${CMAKE_CURRENT_LIST_DIR}/../../build/vsDevCmdCache.${target_arch}.${vs_install_version}.json)
    cmake_path(NORMAL_PATH vsdevcmd_cache_json_path)
    set(${output_variable} ${vsdevcmd_cache_json_path} PARENT_SCOPE)
endfunction()

# -----------------------------------------------------------------------------
# Looks for cached VsDevCmd.bat values set by running vcpkg install or
# outer project cmake generate step
# -----------------------------------------------------------------------------
function (try_set_msvc_environment_variables_from_cache vs_install_version target_arch output_variable)
    message(STATUS "vs_install_version: ${vs_install_version}")
    get_msvc_environment_variables_cache_path(${target_arch} vsdevcmd_cache_json_path)

    message(STATUS "vsdevcmd_cache_json_path: ${vsdevcmd_cache_json_path}")
    if (NOT EXISTS ${vsdevcmd_cache_json_path})
        set(${output_variable} FALSE PARENT_SCOPE)
        message(STATUS "Did not find cached environment variables file '${vsdevcmd_cache_json_path}'.")
        return()
    endif()

    file(READ ${vsdevcmd_cache_json_path} vsdevcmd_cache_json)
    string(JSON json_type TYPE "${vsdevcmd_cache_json}")
    if (NOT "${json_type}" STREQUAL "ARRAY")
        message(FATAL_ERROR "${vsdevcmd_cache_json_path} root element is type ${json_type} instead of ARRAY")
    endif()

    string(JSON variable_count LENGTH "${vsdevcmd_cache_json}")
    math(EXPR variable_count "${variable_count}-1")
    foreach(index RANGE ${variable_count})

        # Check that reference member is a string
        string(JSON json_type TYPE "${vsdevcmd_cache_json}" ${index})
        if (NOT "${json_type}" STREQUAL "OBJECT")
            message(FATAL_ERROR "${vsdevcmd_cache_json_path} array index ${index} is type ${json_type} instead of OBJECT")
            return()
        endif()

        string(JSON json_type TYPE "${vsdevcmd_cache_json}" ${index} Item1)
        if (NOT "${json_type}" STREQUAL "STRING")
            message(FATAL_ERROR "${vsdevcmd_cache_json_path} array index ${index} Item1 is type ${json_type} instead of STRING")
            return()
        endif()

        string(JSON json_type TYPE "${vsdevcmd_cache_json}" ${index} Item2)
        if (NOT "${json_type}" STREQUAL "STRING")
            message(FATAL_ERROR "${vsdevcmd_cache_json_path} array index ${index} Item2 is type ${json_type} instead of STRING")
            return()
        endif()

        # Read the reference member and add it to the list
        string(JSON var_name GET "${vsdevcmd_cache_json}" ${index} Item1)
        string(JSON var_value GET "${vsdevcmd_cache_json}" ${index} Item2)
        message(STATUS "SETTING ${var_name} to ${var_value}")
        set(ENV{${var_name}} "${var_value}")

    endforeach()

    set(${output_variable} TRUE PARENT_SCOPE)
endfunction()

# -----------------------------------------------------------------------------
# Runs VsDevCmd.bat to get the environment variables needed to build with msvc
# and sets the environment variables for the cmake process
# -----------------------------------------------------------------------------
function (set_msvc_environment_variables_from_vsdevcmd vs_install_path target_arch)
    set(vsdevcmd_name "VsDevCmd.bat")
    set(vsdevcmd_path "${vs_install_path}/Common7/Tools/${vsdevcmd_name}")
    cmake_path(NORMAL_PATH vsdevcmd_path)
    message(STATUS "Running ${vsdevcmd_name} to get msvc paths")
    
    if (NOT EXISTS "${vsdevcmd_path}")
        message(FATAL_ERROR "Could not find ${vsdevcmd_name}")
    endif()
    
    if (${target_arch} STREQUAL "x64")
        set(vsdevcmd_target_arch "amd64")
    elseif (${target_arch} STREQUAL "x86")
        set(vsdevcmd_target_arch "x86")
    elseif (${target_arch} STREQUAL "arm64")
        set(vsdevcmd_target_arch "arm64")
    else()
        message(FATAL_ERROR "Unsupported target_arch: ${target_arch}")
    endif()

    execute_process(
        COMMAND cmd.exe /c "${vsdevcmd_path}" -host_arch=amd64 -arch=${vsdevcmd_target_arch} && set
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output_lines
    )

	# Cmake lists are semicolon-delimited, so convert line endings to semicolons.
	# Convert semicolons to bars (|) so they aren't interpreted as line endings
	string(REPLACE ";" "|" output_lines "${output_lines}")
	string(REPLACE "\n" ";" output_lines "${output_lines}")

    set(errors_encountered FALSE)
    set(vsdevcmd_cache_json "[]")
    set(json_array_index 0)

    foreach (output_line ${output_lines})
        string(REPLACE "|" ";" output_line "${output_line}")

        #check for error
        string(REGEX MATCH "^\\[ERROR.+\\].*$" match "${output_line}")
        if (NOT "${match}" STREQUAL "")
            message(STATUS "'${output_line}'")
            set(errors_encountered TRUE)
        else()
            #check for debug output (when running vcdevcmd with $env:VSCMD_DEBUG set to "1", "2", or "3" )
            string(REGEX MATCH "^\\[DEBUG.+\\].*$" match "${output_line}")
            if (NOT "${match}" STREQUAL "")
                message(STATUS "${output_line}")
            else()
                #check for environment variable set
                string(REGEX MATCH "^([^=]+)=(.*)$" match "${output_line}")
                if (NOT "${match}" STREQUAL "")
                    set(var_name "${CMAKE_MATCH_1}")
                    set(var_value "${CMAKE_MATCH_2}")
                    message(VERBOSE "SETTING ${var_name} to ${var_value}")

                    # Set environment variable
                    set(ENV{${var_name}} "${var_value}")

                    # Build json cache value to write
                    string(REPLACE "\\" "\\\\" var_name "${var_name}")
                    string(REPLACE "\\" "\\\\" var_value "${var_value}")
                    string(JSON vsdevcmd_cache_json SET "${vsdevcmd_cache_json}" ${json_array_index} "{}")
                    string(JSON vsdevcmd_cache_json SET "${vsdevcmd_cache_json}" ${json_array_index} Item1 "\"${var_name}\"")
                    string(JSON vsdevcmd_cache_json SET "${vsdevcmd_cache_json}" ${json_array_index} Item2 "\"${var_value}\"")
                    math(EXPR json_array_index "${json_array_index}+1")
                endif()
            endif()
        endif()
    endforeach()

    # Write environment variables to cache file
    #message(STATUS "vsdevcmd_cache_json: ${vsdevcmd_cache_json}")
    get_msvc_environment_variables_cache_path(${target_arch} vsdevcmd_cache_json_path)
    message(STATUS "vsdevcmd_cache_json_path: ${vsdevcmd_cache_json_path}")
    file(WRITE ${vsdevcmd_cache_json_path} "${vsdevcmd_cache_json}")

    if (errors_encountered)
        message(FATAL_ERROR "Errors encountered running ${vsdevcmd_name}")
    endif()

endfunction()

# -----------------------------------------------------------------------------
# Configures environment variables to build with msvc
# -----------------------------------------------------------------------------
function(configure_windows_compiler_environment target_arch)

    # If run from a developer command prompt or Visual Studio, the environment variables will already be set
    if ("${cl}" STREQUAL "cl-NOTFOUND" OR NOT DEFINED ENV{VCINSTALLDIR} OR NOT DEFINED ENV{INCLUDE} OR NOT DEFINED ENV{LIB})
        find_visual_studio_info(vs_install_path vs_install_version)
        try_set_msvc_environment_variables_from_cache(${vs_install_version} ${target_arch} set_from_cache_result)
        if (NOT set_from_cache_result)
            set_msvc_environment_variables_from_vsdevcmd("${vs_install_path}" ${target_arch})
        endif()

        # Configure cmake project to use msvc include and lib directories for build
        include_directories(SYSTEM $ENV{INCLUDE})
        link_directories($ENV{LIB})
    endif()

endfunction()

# -----------------------------------------------------------------------------
# Configures environment variables to build with LLVM clang
# -----------------------------------------------------------------------------
function(configure_macos_compiler_environment)
    # As long as homebrew and llvm are installed, the toolchain file will be
    # able to find clang and set the compiler path
endfunction()

# -----------------------------------------------------------------------------
# Configures environment variables to build with LLVM clang
# -----------------------------------------------------------------------------
function(configure_linux_compiler_environment)
    # As long as llvm package is installed, the toolchain file will be
    # able to find clang and set the compiler path
endfunction()

# -----------------------------------------------------------------------------
# Configures the environment so the preferred compiler can work.
# -----------------------------------------------------------------------------
if (PLATFORM_WIN)
    get_processor_arch(target_arch)
    configure_windows_compiler_environment(${target_arch})
elseif (PLATFORM_MAC)
    configure_macos_compiler_environment()
elseif (PLATFORM_LINUX)
    configure_linux_compiler_environment()
else()
    message(FATAL_ERROR "Unsupported system type")
endif()
