# cmake_minimum_required(VERSION 3.4.3)
# project(Capable-Plugin)

# # LLVM requires C++11.
# set(CMAKE_CXX_STANDARD 11)

# # Find LLVM (this looks for LLVMConfig.cmake somewhere in the PATH or LLVM_DIR).
# find_package(LLVM REQUIRED CONFIG)

# # Enable us to use the LLVM CMake modules.
# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${LLVM_CMAKE_DIR}")

# # Enable us to use add_llvm_library.
# include(AddLLVM)

# # Bring in the LLVM include directories.
# include_directories(${LLVM_INCLUDE_DIRS})

# # Bring in any definitions LLVM requires.
# add_definitions(${LLVM_DEFINITIONS})

add_llvm_library(Capable MODULE Capable.cpp PLUGIN_TOOL clang)

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(Capable PRIVATE
    clangAST
    clangBasic
    clangFrontend
    )
endif()