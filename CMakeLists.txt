cmake_minimum_required(VERSION 3.10)
project(unicode_confusables)

set(CMAKE_CXX_STANDARD 17)

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

# Generate the confusables data header at build time
add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/include/unicode_confusables_data.h
    COMMAND confusables_codegen ${CMAKE_BINARY_DIR}/confusables.txt ${CMAKE_SOURCE_DIR}/include/unicode_confusables_data.h
    DEPENDS confusables_codegen ${CMAKE_BINARY_DIR}/confusables.txt
    COMMENT "Generating unicode_confusables_data.h from confusables.txt"
)
add_custom_target(generate_confusables_header
    DEPENDS ${CMAKE_SOURCE_DIR}/include/unicode_confusables_data.h
)

# Build the main library and tests after header is generated
add_library(unicode_confusables src/unicode_confusables.cpp)
target_include_directories(unicode_confusables PUBLIC include)
target_link_libraries(unicode_confusables PUBLIC ${ICU_LIBRARIES})
add_dependencies(unicode_confusables generate_confusables_header)

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
