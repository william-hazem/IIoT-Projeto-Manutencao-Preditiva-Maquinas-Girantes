# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)

set(EXTRA_COMPONENT_DIRS "../../components")

idf_component_register(SRCS ${app_sources} REQUIRES "esp-cryptoauthlib")

target_add_binary_data(${COMPONENT_TARGET} "client/index.html" BINARY)
target_add_binary_data(${COMPONENT_TARGET} "client/script.js" BINARY)