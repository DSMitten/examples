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
    file(REAL_PATH ${source_path} source_path)

    install(FILES ${source_path} DESTINATION ${CLIENT_APP_FRAMEWORKS_DIR})
    set(install_name_tool_cmd "
        execute_process(COMMAND install_name_tool -change ${source_path} @executable_path/../Frameworks/${shared_library_name} ${CLIENT_APP_EXE_PATH} RESULT_VARIABLE result)
        if (\$\{result\} EQUAL 1)
            message(FATAL_ERROR \"install_name_tool failed\")
        endif()
        ")
    install(CODE "${install_name_tool_cmd}")

endfunction()

# Apple bundle contents placement documentation:
# https://developer.apple.com/documentation/bundleresources/placing-content-in-a-bundle
# Executable
install(TARGETS ${CLIENT_APP_NAME} DESTINATION bin)

install_shared_library(libunwind.1.dylib ${LLVM_LIB_DIR})
install_shared_library(libc++.1.dylib ${LLVM_LIBCPP_DIR})

# WebView2 framework runtime files
install(DIRECTORY ${WEBVIEW2_FRAMEWORK_PATH} DESTINATION ${CLIENT_APP_FRAMEWORKS_DIR} PATTERN "Headers" EXCLUDE)

# WebView2 browser executable app
install(DIRECTORY ${WEBVIEW2_APP_PATH} DESTINATION ${CLIENT_APP_HELPERS_DIR} USE_SOURCE_PERMISSIONS)
