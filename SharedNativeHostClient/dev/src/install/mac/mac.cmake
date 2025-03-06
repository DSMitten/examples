include(${CMAKE_CURRENT_LIST_DIR}/../../../make/mac/MacLlvm.cmake)

get_macos_llvm_lib_dir(LLVM_LIB_DIR)
get_macos_llvm_libcpp_dir(LLVM_LIBCPP_DIR)

set(CLIENT_APP_EXE_PATH ${CMAKE_INSTALL_PREFIX}/bin/${CLIENT_APP_NAME}.app/Contents/MacOS/${CLIENT_APP_NAME})
message(STATUS "CLIENT_APP_EXE_PATH: ${CLIENT_APP_EXE_PATH}")
set(CLIENT_APP_CONTENTS_DIR bin/${CLIENT_APP_NAME}.app/Contents)
set(CLIENT_APP_FRAMEWORKS_DIR ${CLIENT_APP_CONTENTS_DIR}/Frameworks)
set(CLIENT_APP_HELPERS_DIR ${CLIENT_APP_CONTENTS_DIR}/Helpers)
set(SITE_INSTALL_DIR ${CLIENT_APP_HELPERS_DIR})

function (install_shared_library shared_library_name source_dir)

    set(source_path ${source_dir}/${shared_library_name})

    # source_real_path is the path to the actual file if source_path points to a symlink
    file(REAL_PATH ${source_path} source_real_path)

    install(FILES ${source_real_path} DESTINATION ${CLIENT_APP_FRAMEWORKS_DIR})
    set(install_name_tool_cmd "
        set(destination_path @executable_path/../Frameworks/${shared_library_name})
        message(STATUS \"executing command: install_name_tool -change ${source_path} \$\{destination_path\} ${CLIENT_APP_EXE_PATH}\")
        execute_process(COMMAND install_name_tool -change ${source_path} \$\{destination_path\} ${CLIENT_APP_EXE_PATH} RESULT_VARIABLE result)
        if (NOT \$\{result\} EQUAL 0)
            message(FATAL_ERROR \"install_name_tool failed\")
        endif()
        ")
    install(CODE "${install_name_tool_cmd}")

endfunction()

function (validate_bundle_dependencies)
    set(validate_cmd "
        message(STATUS \"executing command: otool -L ${CLIENT_APP_EXE_PATH}\")
        execute_process(COMMAND otool -L ${CLIENT_APP_EXE_PATH} RESULT_VARIABLE result OUTPUT_VARIABLE output)
        if (NOT \$\{result\} EQUAL 0)
            message(FATAL_ERROR \"otool failed\")
        endif()
        message(STATUS \"\$\{output\}\\n\\n--------------\\n\")
        string(STRIP \"\$\{output\}\" output)
        string(REPLACE \"\\n\" \";\" output_lines \"\$\{output\}\")
        list(REMOVE_AT output_lines 0)
        foreach(line \$\{output_lines\})
            if (NOT line MATCHES \"\\\\s*/usr/lib/\" AND
                NOT line MATCHES \"\\\\s*/System/\" AND
                NOT line MATCHES \"\\\\s*@executable_path/\")
                message(FATAL_ERROR \"Invalid dependency path found in ${CLIENT_APP_NAME}:\\n\$\{line\}\")
            endif()
        endforeach()
        ")
    install(CODE "${validate_cmd}")
    
endfunction()

# Apple bundle contents placement documentation:
# https://developer.apple.com/documentation/bundleresources/placing-content-in-a-bundle
# Executable
install(TARGETS ${CLIENT_APP_NAME} DESTINATION bin)

install_shared_library(libunwind.1.dylib ${LLVM_LIB_DIR})
install_shared_library(libc++.1.dylib ${LLVM_LIBCPP_DIR})

# Make sure that all dependencies are either system files or in the bundle
validate_bundle_dependencies()

# WebView2 framework runtime files
install(DIRECTORY ${WEBVIEW2_FRAMEWORK_PATH} DESTINATION ${CLIENT_APP_FRAMEWORKS_DIR} PATTERN "Headers" EXCLUDE)

# WebView2 browser executable app
install(DIRECTORY ${WEBVIEW2_APP_PATH} DESTINATION ${CLIENT_APP_HELPERS_DIR} USE_SOURCE_PERMISSIONS)
