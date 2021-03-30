cmake_minimum_required(VERSION 3.5)

project(UTILS_LIB)

include(CheckIncludeFileCXX)

check_include_file_cxx(any HAS_ANY)
check_include_file_cxx(string_view HAS_STRING_VIEW)
check_include_file_cxx(ranges HAS_RANGES)

set(CMAKE_CXX_STANDARD 17)

if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif ()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../lib_database lib_database)

add_library(${PROJECT_NAME} STATIC)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/utils UTIL_SRC)

find_package(OpenSSL REQUIRED)
find_package(cryptopp CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC
        OpenSSL::SSL
        OpenSSL::Crypto
        cryptopp-static
        DATABASE_LIB)

target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_sources(${PROJECT_NAME} PUBLIC ${UTIL_SRC})
