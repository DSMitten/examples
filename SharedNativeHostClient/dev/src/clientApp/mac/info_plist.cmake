# Copyright (C) Microsoft Corporation. All rights reserved.

# Set properties that will affect how CMake configures the info.plist file

# Variables need to be set in parent scope so they are visible in the CMakeLists.txt file.
# This is called from add_gui_executable, which is a function, and we don't want the variable scope to be that function
#set(APP_ICON_FILE ${APP_ICON}.icns)

#set(MACOSX_BUNDLE_ICON_FILE ${APP_ICON_FILE} PARENT_SCOPE)
