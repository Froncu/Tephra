cmake_minimum_required(VERSION 3.30)
project(tephra LANGUAGES CXX)

include(GNUInstallDirs)
include(target_defaults)

find_package(LibXml2 CONFIG REQUIRED)
find_package(ZLIB CONFIG REQUIRED)
find_package(zstd CONFIG REQUIRED)
find_package(LLVM CONFIG REQUIRED)
cmake_path(GET LLVM_CMAKE_DIR PARENT_PATH LLVM_CMAKE_PARENT_DIR)
set(Clang_DIR ${LLVM_CMAKE_PARENT_DIR}/clang)
find_package(Clang CONFIG REQUIRED NO_DEFAULT_PATH)

file(GLOB_RECURSE TEP_SOURCE_FILES CONFIGURE_DEPENDS source/*.cpp)
add_executable(${PROJECT_NAME} ${TEP_SOURCE_FILES})

target_include_directories(${PROJECT_NAME} SYSTEM
   PRIVATE ${LLVM_INCLUDE_DIRS}
   PRIVATE ${CLANG_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME}
   PRIVATE source)

target_precompile_headers(${PROJECT_NAME}
   PRIVATE source/pch.hpp)

separate_arguments(TEPHRA_LLVM_DEFINITIONS NATIVE_COMMAND ${LLVM_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PRIVATE ${TEPHRA_LLVM_DEFINITIONS})

target_link_libraries(${PROJECT_NAME}
   PRIVATE clangTooling
   PRIVATE clangASTMatchers
   PRIVATE clangBasic)

set_target_defaults(${PROJECT_NAME})

install(
   TARGETS ${PROJECT_NAME}
   EXPORT tephraTargets
   RUNTIME
   DESTINATION ${CMAKE_INSTALL_BINDIR})
install(
   EXPORT tephraTargets
   FILE tephraTargets.cmake
   NAMESPACE tephra::
   DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME})
install(
   FILES cmake/tephraConfig.cmake
   DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME})