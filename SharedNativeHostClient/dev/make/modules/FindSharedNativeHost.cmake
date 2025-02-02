# Create the header library target
function(setup_include_target)
    set(include_target_name NativeHostIncludeDirs)
    add_library(${include_target_name} INTERFACE)
    target_include_directories(${include_target_name} 
        INTERFACE 
        ${VCPKG_INC_DIR}
    )

    add_library(NativeHost::IncludeDirs ALIAS ${include_target_name})

endfunction()

# Set up the NativeHost::SharedLibrary target
function(setup_shared_library_target)
    # Create the shared library target
    set(target_name NativeHostSharedLibrary)
    add_library(${target_name} SHARED IMPORTED)
    target_link_libraries(${target_name} 
        INTERFACE 
            NativeHost::IncludeDirs
    )

    if(PLATFORM_WIN)
        set_target_properties(${target_name}
            PROPERTIES
            IMPORTED_LOCATION ${VCPKG_BIN_DIR}/nh.dll
            IMPORTED_IMPLIB ${VCPKG_LIB_DIR}/nh.lib
        )

        target_link_options(${target_name} INTERFACE "/DELAYLOAD:nh.dll")

        set(SHARED_NATIVE_HOST_INSTALL_BIN_FILES ${VCPKG_BIN_DIR}/nh.dll ${VCPKG_LIB_DIR}/nh.lib PARENT_SCOPE)
        set(SHARED_NATIVE_HOST_INSTALL_SYMBOL_FILES ${VCPKG_BIN_DIR}/nh.pdb PARENT_SCOPE)
    elseif(PLATFORM_MAC)
        target_link_libraries(${target_name}
            INTERFACE
                MacOS::Foundation)

        set_target_properties(${target_name}
            PROPERTIES
            IMPORTED_LOCATION ${VCPKG_LIB_DIR}/libnh.dylib
        )
        set(SHARED_NATIVE_HOST_INSTALL_BIN_FILES ${VCPKG_LIB_DIR}/libnh.dylib PARENT_SCOPE)
    elseif(PLATFORM_LINUX)
         set_target_properties(${target_name}
            PROPERTIES
            IMPORTED_LOCATION ${VCPKG_LIB_DIR}/libnh.so
        )
        set(SHARED_NATIVE_HOST_INSTALL_BIN_FILES ${VCPKG_LIB_DIR}/libnh.so PARENT_SCOPE)
    endif()

    add_library(NativeHost::SharedLibrary ALIAS ${target_name})
 
endfunction()

# Finds the share directory for ms-wv2host-tools
function(get_shared_files_dir)
    if (PLATFORM_WIN)
        if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
            set(vcpkg_host_triplet "x64-windows")
        endif()
    elseif (PLATFORM_MAC)
        if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "arm64")
            set(vcpkg_host_triplet "arm64-osx")
        elseif (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "x86_64")
            set(vcpkg_host_triplet "x64-osx")
        endif()
    elseif (PLATFORM_LINUX)
        if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "arm64")
            set(vcpkg_host_triplet "arm64-linux")
        elseif (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "x86_64")
            set(vcpkg_host_triplet "x64-linux")
        endif()
    endif()

    if (NOT DEFINED vcpkg_host_triplet)
        message(FATAL_ERROR "Unknown host system processor ${CMAKE_HOST_SYSTEM_PROCESSOR}")
    endif()

    set(MS_WV2HOST_TOOLS_SHARE_DIR "${VCPKG_INSTALLED_DIR}/${vcpkg_host_triplet}/share/ms-wv2host-tools" PARENT_SCOPE)
endfunction()

# Ensure shared native host is on the system and sets up the NativeHost::IncludeDirs and NativeHost::SharedLibrary targets
function(setup_shared_native_host_framework_targets)
    message(VERBOSE "Setting up shared native host targets")

    setup_include_target()
    setup_shared_library_target()
    get_shared_files_dir()

    set(SHARED_NATIVE_HOST_INSTALL_BIN_FILES ${SHARED_NATIVE_HOST_INSTALL_BIN_FILES} PARENT_SCOPE)
    set(SHARED_NATIVE_HOST_INSTALL_SYMBOL_FILES ${SHARED_NATIVE_HOST_INSTALL_SYMBOL_FILES} PARENT_SCOPE)
    set(MS_WV2HOST_TOOLS_SHARE_DIR ${MS_WV2HOST_TOOLS_SHARE_DIR} PARENT_SCOPE)

    message(VERBOSE "SharedNativeHost FOUND")
    set(SharedNativeHost_FOUND TRUE PARENT_SCOPE) 
endfunction()

setup_shared_native_host_framework_targets()
