# Copyright (C) Microsoft Corporation. All rights reserved.

# -----------------------------------------------------------------------------
# Gets variables for running a shell command, which depends on the platform
# -----------------------------------------------------------------------------
function (get_shell_command shell_var command_switch_var)

if (PLATFORM_WIN)
        set("${shell_var}" "cmd" PARENT_SCOPE)
        set("${command_switch_var}" "/c" PARENT_SCOPE)
    elseif(PLATFORM_MAC OR PLATFORM_LINUX)
        set("${shell_var}" "bash" PARENT_SCOPE)
        set("${command_switch_var}" "-c" PARENT_SCOPE)
    endif()

endfunction()

# -----------------------------------------------------------------------------
# Runs the npm install command to ensure that all dependencies are installed
# -----------------------------------------------------------------------------
function (install_node_modules)

    get_shell_command(shell command_switch)

    message(STATUS "npm install --no-fund")
    execute_process(
        COMMAND ${shell} ${command_switch} "npm install --no-fund"
        RESULT_VARIABLE result						# result contains the exit code for the process
        WORKING_DIRECTORY "${BUILD_DIR}"
    )

    if (NOT ${result} EQUAL 0)
        # npm's output suggests fixes for authentication errors that are unclear and confusing for our usecase, but we have our own solutions in mcheck.
        message(FATAL_ERROR 
            "Failed to install npm dependencies: ${result}\n"
            "If the above error is related to authentication, try running mcheck to resolve."
        )
    endif()

endfunction()

function (add_npm_build_target)

        get_shell_command(shell command_switch)
        add_custom_target(
        npm-run-build
        ALL
        COMMAND ${shell} ${command_switch} "npm run build -- --interopgen ${INTEROPGEN}"
    )

endfunction()