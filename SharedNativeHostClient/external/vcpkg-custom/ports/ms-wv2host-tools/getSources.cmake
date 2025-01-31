# Shared function to clone down the sources for the WebView2 host framework,
# used by both ms-wv2host-tools and ms-wv2host ports.
function(get_sources source_path_variable_name)
    vcpkg_from_git(
        OUT_SOURCE_PATH source_path
        URL https://github.com/microsoft/webview2-host-framework
        REF 9a6e17e73c481384d349462fdd34f771d35a5549 # release v0.11.0
    )

    set(${source_path_variable_name} ${source_path} PARENT_SCOPE)
endfunction()
