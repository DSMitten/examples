include(${CMAKE_CURRENT_LIST_DIR}/../ClangVersion.cmake)

function(get_macos_llvm_path output_variable)
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

    set(${output_variable} ${llvm_path} PARENT_SCOPE)
endfunction()

function(get_macos_llvm_lib_dir output_variable)
    get_macos_llvm_path(llvm_path)
    set(${output_variable} ${llvm_path}/lib PARENT_SCOPE)
endfunction()

function(get_macos_llvm_libcpp_dir output_variable)
    get_macos_llvm_path(llvm_path)
    set(${output_variable} ${llvm_path}/lib/c++ PARENT_SCOPE)
endfunction()
