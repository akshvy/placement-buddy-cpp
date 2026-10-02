cmake_minimum_required(VERSION 3.14)
project(PlacementBuddy CXX)

set(CMAKE_CXX_STANDARD 17)

# 1. Find system libcurl without loading package configuration dependencies
find_path(CURL_INCLUDE_DIR curl/curl.h REQUIRED)
find_library(CURL_LIBRARY NAMES curl REQUIRED)

add_library(CURL::libcurl UNKNOWN IMPORTED)
set_target_properties(CURL::libcurl PROPERTIES
    IMPORTED_LOCATION "${CURL_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${CURL_INCLUDE_DIR}"
)

# 2. Automatically download nlohmann_json header library
include(FetchContent)
# Always use the declared download instead of looking for an installed
# nlohmann_json package configuration file.
set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE NEVER)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

# 3. Target definition
add_executable(placement_buddy main.cpp)

# 4. Link dependencies
target_link_libraries(placement_buddy PRIVATE 
    CURL::libcurl 
    nlohmann_json::nlohmann_json
)