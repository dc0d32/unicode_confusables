cmake_minimum_required(VERSION 3.10)
project(unicode_confusables)

set(CMAKE_CXX_STANDARD 17)

# Compiler optimizations for faster build times
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Add build time optimizations
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1")  # Use -O1 instead of -O0 for better template instantiation
    if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR NOT CMAKE_BUILD_TYPE)
        # For debug builds, prioritize compile time
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g1")  # Reduced debug info for faster compilation
    endif()
endif()

# Use parallel build by default
if(NOT DEFINED CMAKE_BUILD_PARALLEL_LEVEL)
    include(ProcessorCount)
    ProcessorCount(N)
    if(NOT N EQUAL 0)
        set(CMAKE_BUILD_PARALLEL_LEVEL ${N} CACHE STRING "Number of parallel build jobs")
    endif()
endif()

# Download confusables.txt at configure time (platform-agnostic, skip if exists)
if(NOT EXISTS "${CMAKE_BINARY_DIR}/confusables.txt")
    file(DOWNLOAD
        https://www.unicode.org/Public/security/latest/confusables.txt
        ${CMAKE_BINARY_DIR}/confusables.txt
        SHOW_PROGRESS
        STATUS confusables_download_status
    )
endif()

find_package(ICU REQUIRED COMPONENTS uc i18n)
include_directories(${ICU_INCLUDE_DIRS})
link_directories(${ICU_LIBRARY_DIRS})

# Build codegen tool first
add_executable(confusables_codegen tools/confusables_codegen.cpp)
target_include_directories(confusables_codegen PRIVATE include)
target_link_libraries(confusables_codegen PRIVATE ${ICU_LIBRARIES})

# Generate the confusables data header and cpp files at build time
add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/include/unicode_confusables_data.h ${CMAKE_SOURCE_DIR}/src/unicode_confusables_data.cpp
    COMMAND confusables_codegen ${CMAKE_BINARY_DIR}/confusables.txt ${CMAKE_SOURCE_DIR}/include/unicode_confusables_data.h ${CMAKE_SOURCE_DIR}/src/unicode_confusables_data.cpp
    DEPENDS confusables_codegen ${CMAKE_BINARY_DIR}/confusables.txt
    COMMENT "Generating unicode_confusables_data.h and unicode_confusables_data.cpp from confusables.txt"
)
add_custom_target(generate_confusables_header
    DEPENDS ${CMAKE_SOURCE_DIR}/include/unicode_confusables_data.h ${CMAKE_SOURCE_DIR}/src/unicode_confusables_data.cpp
)

# Build the main library and tests after header is generated
add_library(unicode_confusables src/unicode_confusables.cpp src/unicode_confusables_data.cpp)
target_include_directories(unicode_confusables PUBLIC include)
target_link_libraries(unicode_confusables PUBLIC ${ICU_LIBRARIES})
add_dependencies(unicode_confusables generate_confusables_header)

# Special optimization for the large data file
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Reduce optimization level for the large data file to speed up compilation
    set_source_files_properties(src/unicode_confusables_data.cpp PROPERTIES 
        COMPILE_FLAGS "-O0 -g0 -fno-var-tracking"  # No optimization, no debug info, no variable tracking for faster compilation
        SKIP_PRECOMPILE_HEADERS ON)  # Skip PCH for this file to avoid optimization mismatch
    
    # Use precompiled headers if available (CMake 3.16+) for other files
    if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.16")
        target_precompile_headers(unicode_confusables PRIVATE 
            <unordered_map>
            <unordered_set>
            <string>
        )
    endif()
endif()

add_executable(test_confusables tests/test_confusables.cc)
target_include_directories(test_confusables PRIVATE include)
target_link_libraries(test_confusables PRIVATE unicode_confusables ${ICU_LIBRARIES})
add_dependencies(test_confusables generate_confusables_header)

# Console application for normalizing confusables
add_executable(confusables_normalize apps/confusables_normalize.cpp)
target_include_directories(confusables_normalize PRIVATE include)
target_link_libraries(confusables_normalize PRIVATE unicode_confusables ${ICU_LIBRARIES})
add_dependencies(confusables_normalize generate_confusables_header)

enable_testing()
add_test(NAME ConfusablesTest COMMAND test_confusables)

# Build bindings (optional)
option(BUILD_CSHARP_BINDINGS "Build C# bindings" OFF)
option(BUILD_PYTHON_BINDINGS "Build Python bindings" OFF)

# C# bindings
if(BUILD_CSHARP_BINDINGS)
    # Build the C wrapper library for C# bindings
    add_library(unicode_confusables_csharp SHARED 
        bindings/csharp/unicode_confusables_c.cpp
        src/unicode_confusables.cpp 
        src/unicode_confusables_data.cpp
    )
    target_include_directories(unicode_confusables_csharp PUBLIC 
        include
        bindings/csharp
    )
    target_link_libraries(unicode_confusables_csharp PUBLIC ${ICU_LIBRARIES})
    add_dependencies(unicode_confusables_csharp generate_confusables_header)
    
    # Set the library name for consistent cross-platform usage
    set_target_properties(unicode_confusables_csharp PROPERTIES
        OUTPUT_NAME "unicode_confusables_csharp"
        PREFIX ""
    )
endif()

# Python bindings
if(BUILD_PYTHON_BINDINGS)
    find_package(pybind11 QUIET)
    if(pybind11_FOUND)
        pybind11_add_module(unicode_confusables_py 
            bindings/python/unicode_confusables_py.cpp
            src/unicode_confusables.cpp 
            src/unicode_confusables_data.cpp
        )
        target_include_directories(unicode_confusables_py PRIVATE include)
        target_link_libraries(unicode_confusables_py PRIVATE ${ICU_LIBRARIES})
        add_dependencies(unicode_confusables_py generate_confusables_header)
        
        # Set properties for the Python module
        target_compile_definitions(unicode_confusables_py PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})
    else()
        message(WARNING "pybind11 not found. Python bindings will not be built. Install pybind11 and reconfigure to enable Python bindings.")
    endif()
endif()
