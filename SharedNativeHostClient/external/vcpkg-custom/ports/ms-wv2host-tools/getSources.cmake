# Shared function to clone down the sources for the WebView2 host framework,
# used by both ms-wv2host-tools and ms-wv2host ports.
function(get_sources source_path_variable_name)
    vcpkg_from_git(
        OUT_SOURCE_PATH source_path
        URL https://github.com/microsoft/webview2-host-framework
        REF c96f082fe14638bd260efb5931df3e38615c346d # release v0.12.5
    )

    set(${source_path_variable_name} ${source_path} PARENT_SCOPE)
endfunction()
