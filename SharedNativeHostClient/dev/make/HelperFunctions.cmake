# -----------------------------------------------------------------------------
# Takes a cmake path variable, converts it to a native path, and sets a compile
# definition containing the value. For tests and example code that need to
# reference paths in the repo.
# -----------------------------------------------------------------------------
function (target_path_compile_definition target_name path_value variable_name)

    file(TO_NATIVE_PATH "${path_value}" ${variable_name})
    if (PLATFORM_WIN)
        string(REPLACE "\\" "\\\\" ${variable_name} "${${variable_name}}")
    else()
        # TO_NATIVE_PATH escapes spaces with '\ ', which isn't necessary when setting definitions with "-DVAR=\"value\""
        string(REPLACE "\\ " " " ${variable_name} "${${variable_name}}")
    endif()

    target_compile_definitions(${target_name}
        PRIVATE
            ${variable_name}="${${variable_name}}"
    )

endfunction()
