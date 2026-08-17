cmake_minimum_required(VERSION 3.30)
project(tephra LANGUAGES CXX)

include(GNUInstallDirs)

find_package(LibXml2 REQUIRED)
find_package(ZLIB REQUIRED)
find_package(zstd CONFIG REQUIRED)
if (TARGET zstd::libzstd AND NOT TARGET zstd::libzstd_static)
   add_library(zstd::libzstd_static ALIAS zstd::libzstd)
endif ()

find_package(LLVM REQUIRED CONFIG)

cmake_path(GET LLVM_CMAKE_DIR PARENT_PATH TEPHRA_LLVM_CMAKE_PARENT)
set(Clang_DIR ${TEPHRA_LLVM_CMAKE_PARENT}/clang)
set(TEPHRA_CLANG_CONFIG_VERSION ${LLVM_VERSION_MAJOR}.${LLVM_VERSION_MINOR}.${LLVM_VERSION_PATCH})
find_package(Clang ${TEPHRA_CLANG_CONFIG_VERSION} EXACT REQUIRED CONFIG NO_DEFAULT_PATH)

add_executable(${PROJECT_NAME} source/main.cpp)

target_include_directories(${PROJECT_NAME} SYSTEM
   PRIVATE ${LLVM_INCLUDE_DIRS}
   PRIVATE ${CLANG_INCLUDE_DIRS})

separate_arguments(TEPHRA_LLVM_DEFINITIONS NATIVE_COMMAND ${LLVM_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PRIVATE ${TEPHRA_LLVM_DEFINITIONS})

target_link_libraries(${PROJECT_NAME}
   PRIVATE clangTooling
   PRIVATE clangASTMatchers
   PRIVATE clangBasic)

set_target_properties(${PROJECT_NAME} PROPERTIES
   CXX_STANDARD 23
   CXX_STANDARD_REQUIRED TRUE)

target_compile_options(${PROJECT_NAME}
   PRIVATE $<$<CXX_COMPILER_FRONTEND_VARIANT:GNU>:-Wall;-Wextra;-Wpedantic;-Werror;-Wno-braced-scalar-init>
   PRIVATE $<$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>:/W4;/WX>)

configure_file(cmake/tephraConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/tephraConfig.cmake @ONLY)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/tephraConfig.cmake DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME})