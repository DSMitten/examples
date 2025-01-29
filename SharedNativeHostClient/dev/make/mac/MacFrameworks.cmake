# Copyright (C) Microsoft Corporation. All rights reserved.

# -----------------------------------------------------------------------------
# Locates a MacOS framework library with caller-specified conditions
# -----------------------------------------------------------------------------
function(find_macos_custom_framework framework_name framework_dir target_name target_alias_name)

    set(library_name FRAMEWORK_${framework_name}_lib)

    # Find the framework library. find_library will populate ${library_name} with the path to the framework
    # e.g. for Foundation, ${${library_name}} = ${FRAMEWORK_Foundation_lib} = /Library/Developer/CommandLineTools/SDKs/MacOSX12.3.sdk/System/Library/Frameworks/Foundation.framework
    find_library(${library_name} NAME ${framework_name} PATHS ${framework_dir} NO_DEFAULT_PATH)
    if ("${${library_name}}" STREQUAL "${library_name}-NOTFOUND")
        message(FATAL_ERROR "Could not find framework ${framework_name} at ${framework_dir}")
    endif()

    # Create a target that will link against the dependency
    add_library(${target_name} INTERFACE)
    target_link_libraries(${target_name} INTERFACE ${${library_name}})

    # Create an alias target with a more usable name
    add_library(${target_alias_name} ALIAS ${target_name})

    message(VERBOSE "${target_alias_name} FOUND")
    set(${framework_name}_FOUND TRUE PARENT_SCOPE)

endfunction()

# -----------------------------------------------------------------------------
# Locates one of the MacOS framework libraries, and sets a FRAMEWORK_<name>
# variable that can be used to include its files and link against it.
# -----------------------------------------------------------------------------
function(find_macos_framework framework_name)

    set(framework_dir ${CMAKE_OSX_SYSROOT}/System/Library/Frameworks)
    set(target_name MacOS_${framework_name})
    set(target_alias_name MacOS::${framework_name})

    find_macos_custom_framework(${framework_name} ${framework_dir} ${target_name} ${target_alias_name})
    if (${framework_name}_FOUND)
        set(${framework_name}_FOUND TRUE PARENT_SCOPE)
    endif()

endfunction()

find_macos_framework(AppKit)
find_macos_framework(Foundation)
find_macos_framework(SystemConfiguration)
find_macos_framework(Network)
