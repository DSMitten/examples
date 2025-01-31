# -----------------------------------------------------------------------------------------------------
# Ensures WebView2 is on the system.
# Provides an interface target and a shared library for the WebView2 library, a
#
# To include headers only, you want the `WebView2::IncludeDirs` interface library
# add:
# target_link_libraries(${TARGET_NAME} PRIVATE|PUBLIC|INTERFACE WebView2::IncludeDirs)
#
# To link to the WebView2 implementation and get the headers, use the WebView2::WebView2 target
# add:
# target_link_libraries(${TARGET_NAME} PRIVATE|PUBLIC|INTERFACE WebView2::WebView2)
#
# To add the required files for an install command, use the
# WEBVIEW2_INSTALL_FILES variable
# ----------------------------------------------------------------------------------------------------

# Uses nuget to download a package if it is not installed
function(ensure_nuget_package package_name package_version package_source dir_variable_name)

    find_program(NUGET nuget REQUIRED)
    set(nuget_dir ${CMAKE_CURRENT_LIST_DIR}/../../../external/nuget)

	set(package_dir ${nuget_dir}/${package_name}.${package_version})
	set(${dir_variable_name} ${package_dir} PARENT_SCOPE)

	if (IS_DIRECTORY ${package_dir})
		message(VERBOSE "${package_name} version ${package_version} exists.")
	else()
        message(VERBOSE "${NUGET} install ${package_name} -Version ${package_version} -Source ${package_source} -OutputDirectory ${nuget_dir} -Verbosity Quiet")
		execute_process(
			COMMAND ${NUGET} install ${package_name} -Version ${package_version} -Source ${package_source} -OutputDirectory ${nuget_dir} -Verbosity Quiet
			RESULT_VARIABLE nuget_result
			ERROR_VARIABLE errorString
			WORKING_DIRECTORY "${BUILD_DIR}"
		)

		if (${nuget_result} EQUAL 0)
		    message(STATUS "Installed ${package_name} ${package_version} to ${package_dir}")
		else()
		    message(FATAL_ERROR "Failed to install ${package_name} with nuget:\n${errorString}")
	    endif()
    endif()
endfunction()

# Setup the WebView2::IncludeDirs target
function(setup_webview2_win_headers_target module_dir)
    set(module_target_name WebView2_IncludeDirs)

    add_library(${module_target_name} INTERFACE)

    set(WEBVIEW2_INCLUDE_DIR ${module_dir}/include PARENT_SCOPE)
    target_include_directories(${module_target_name} 
        SYSTEM INTERFACE
            ${module_dir}/include
    )

    # Interface libraries cannot use the 'foo::bar' naming pattern, but alias targets can
    add_library(WebView2::IncludeDirs ALIAS ${module_target_name})
endfunction()

# Setup the WebView2::WebView2 target
function(setup_webview2_win_target module_dir)

    set(module_target_name WebView2::WebView2)

    # ===== Config settings =====
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
        set(module_arch x64)
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "i[3-6]86|x86")
        set(module_arch x86)
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
        set(module_arch arm64)
    else()
        message(FATAL_ERROR "Processor architecture not supported")
    endif()

    set(module_lib_dir ${module_dir}/${module_arch})

    # ===== WebView2::WebView2 =====

    add_library(${module_target_name} SHARED IMPORTED)

    set_target_properties(${module_target_name} PROPERTIES
        IMPORTED_LOCATION ${module_lib_dir}/WebView2Loader.dll
        IMPORTED_IMPLIB ${module_lib_dir}/WebView2Loader.dll.lib
    )

    target_link_options(${module_target_name} INTERFACE "/DELAYLOAD:WebView2Loader.dll")

    # Export the include dirs
    target_link_libraries(${module_target_name} INTERFACE WebView2::IncludeDirs)

    # WEBVIEW2_INSTALL_FILES is used in an install() command
    # We set it at the parent scope so the variable won't go away when the function exits.
    set(WEBVIEW2_INSTALL_FILES
        ${module_lib_dir}/WebView2Loader.dll
        PARENT_SCOPE)

endfunction()

# Set up the WebView2::WebView2 target
function(setup_webview2_mac_framework framework_dir)

    set(framework_name MSWebView2)
    find_macos_custom_framework(${framework_name} ${framework_dir} WebView2_WebView2 WebView2::WebView2)

endfunction()

# Ensure WebView2 is on the system and setup both targets
function(setup_webview2_targets)
    # ===== Nuget package =====

    # The WebView2 nuget package contains binaries for win32 and uwp platforms only.
    # For this Webview2 version (https://docs.microsoft.com/en-us/microsoft-edge/webview2/release-notes?tabs=win32cpp#101305-prerelease):
    # Minimum edge version to load is 86.0.616.0 or newer.
    # Edge version for full API compatibility is 105.0.1305.0 or newer
    # If the webview runtime version is below Full compatibility version, some interfaces may not be supported and it is preferred to allow QI
    # to fail silently and start succeeding when the Full compatibility version gets promoted to stable version.
    # This is safer than updating the requirement for minimum edge version to a non-stable edge version here.
    # More information - https://docs.microsoft.com/en-us/microsoft-edge/webview2/concepts/versioning
    if(PLATFORM_WIN)

        ensure_nuget_package("Microsoft.Web.WebView2" "1.0.2957.106" "https://api.nuget.org/v3/index.json" module_dir)
        set(module_dir ${module_dir}/build/native)
        setup_webview2_win_headers_target(${module_dir})
        set(WEBVIEW2_INCLUDE_DIR ${WEBVIEW2_INCLUDE_DIR} PARENT_SCOPE)
        setup_webview2_win_target(${module_dir})
        set(was_found TRUE)
    
    elseif(PLATFORM_MAC)
        # Mac WV2 SDK requires manual install. Default location is in the 'external/nuget' directory
        # but CUSTOM_WV2_FRAMEWORK_DIR can be set to override this directory
        set(framework_dir ${CMAKE_CURRENT_LIST_DIR}/../../../external/nuget/MSWebView2)
        cmake_path(NORMAL_PATH framework_dir)

        if (IS_DIRECTORY ${framework_dir})
            setup_webview2_mac_framework(${framework_dir})
            set(WEBVIEW2_APP_PATH "${framework_dir}/Microsoft Edge WebView2.app" PARENT_SCOPE)
            list(APPEND CMAKE_INSTALL_RPATH "${framework_dir}")
            set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}" PARENT_SCOPE)
            set(was_found TRUE)
        else()
            message(WARNING "WebView2 SDK must be installed manually from https://dev.azure.com/microsoft/Edge/_artifacts/feed/webview2-mac to '${framework_dir}'")
            set(was_found FALSE)
        endif()

    elseif(PLATFORM_LINUX)

        message(FATAL_ERROR "WebView2 is not available for Linux platform builds")

    endif()


    # Used for comparing the installed version against the minimum version required by the app.
    # https://docs.microsoft.com/en-us/microsoft-edge/webview2/releasenotes#10721-prerelease
    set (WEBVIEW_VERSION_MINIMUM_EDGE_VERSION "86.0.616.0" PARENT_SCOPE)

    # Re-export set vars in parent scope
    set(WEBVIEW2_INSTALL_FILES ${WEBVIEW2_INSTALL_FILES} PARENT_SCOPE)

    if (${was_found})
        message(VERBOSE "WebView2 FOUND")
    endif()
    set(WebView2_FOUND ${was_found} PARENT_SCOPE)

endfunction()

setup_webview2_targets()
