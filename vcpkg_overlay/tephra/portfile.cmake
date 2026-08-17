vcpkg_cmake_configure(SOURCE_PATH ${CMAKE_CURRENT_LIST_DIR}/../..)
vcpkg_cmake_install()
vcpkg_copy_tools(TOOL_NAMES tephra AUTO_CLEAN)