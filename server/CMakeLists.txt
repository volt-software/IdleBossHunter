cmake_minimum_required(VERSION 3.13)
project(ibh_server)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE CXX STANDARD REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Debug CACHE STRING
            "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
            FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/server/bin)
set(TOP_DIR ${CMAKE_SOURCE_DIR})
set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/external/)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DXXH_INLINE_ALL -DXXH_CPU_LITTLE_ENDIAN=1 -DRAPIDJSON_SSE42 -DSPDLOG_COMPILED_LIB -DCATCH_CONFIG_FAST_COMPILE -DSPDLOG_NO_EXCEPTIONS -DASIO_STANDALONE -DASIO_CONCURRENCY_HINT_DEFAULT=1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-variable -Wno-long-long -Wno-unused-parameter -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference -pedantic -Wformat -Wformat-security ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconcepts ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx -maes -mpclmul -mpopcnt -msse4.1 -msse4.2 -mrdrnd -mf16c -mfsgsbase -mfxsr -mxsave -mxsaveopt -fstack-protector-strong -fstack-clash-protection -fcf-protection -fPIE")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -ggdb -Og")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")

option(USE_SANITIZERS "Compile with sanitizers, slowing down the build and the runtime" OFF)
IF(USE_SANITIZERS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined ") #-fsanitize=undefined -fsanitize=address -fsanitize=thread ")
ENDIF()

SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -Wl,-O1 -Wl,-z,defs -Wl,-z,now -Wl,-z,relro -Wl,-pie -Wl,-z,noexecstack")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")

# define targets
file(GLOB_RECURSE PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE COMMON_SOURCES ${TOP_DIR}/common/src/*.cpp)
file(GLOB SPDLOG_SOURCE ${EXTERNAL_DIR}/spdlog/src/*.cpp)
add_executable(ibh_server ${SPDLOG_SOURCE} ${PROJECT_SOURCES} ${COMMON_SOURCES})

file(GLOB_RECURSE PROJECT_SOURCES_WITHOUT_MAIN ${PROJECT_SOURCE_DIR}/src/*.cpp)
list(REMOVE_ITEM PROJECT_SOURCES_WITHOUT_MAIN "${PROJECT_SOURCE_DIR}/src/main.cpp")
file(GLOB_RECURSE PROJECT_TEST_SOURCES ${PROJECT_SOURCE_DIR}/test/*.cpp)
add_executable(ibh_server_test ${SPDLOG_SOURCE} ${PROJECT_SOURCES_WITHOUT_MAIN} ${COMMON_SOURCES} ${PROJECT_TEST_SOURCES})
add_test(NAME MyTest COMMAND ibh_server_test)

file(GLOB_RECURSE PROJECT_SOURCES_WITHOUT_MAIN ${PROJECT_SOURCE_DIR}/src/*.cpp)
list(REMOVE_ITEM PROJECT_SOURCES_WITHOUT_MAIN "${PROJECT_SOURCE_DIR}/src/main.cpp")
file(GLOB PROJECT_BENCHMARK_SOURCES ${PROJECT_SOURCE_DIR}/benchmark/*.cpp ${PROJECT_SOURCE_DIR}/benchmark/benchmark_helpers/*.cpp)
add_executable(ibh_benchmark ${SPDLOG_SOURCE} ${PROJECT_SOURCES_WITHOUT_MAIN} ${COMMON_SOURCES} ${PROJECT_BENCHMARK_SOURCES})

target_compile_definitions(ibh_server_test PRIVATE TEST_CODE=1)

find_library(PQXX_LIBRARY pqxx HINTS ${EXTERNAL_DIR}/libpqxx/src/.libs)
find_library(SODIUM_LIBRARY sodium HINTS ${EXTERNAL_DIR}/libsodium/src/libsodium/.libs)

if(NOT PQXX_LIBRARY)
    message(FATAL_ERROR "libpqxx not found")
endif()

if(NOT SODIUM_LIBRARY)
    message(FATAL_ERROR "libsodium not found")
endif()

find_package(ZLIB REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories("${TOP_DIR}/common/src")
include_directories("${TOP_DIR}/server/src")
include_directories("${EXTERNAL_DIR}/spdlog/include")
include_directories("${EXTERNAL_DIR}/rapidjson/include")
include_directories("${EXTERNAL_DIR}/websocketpp")
include_directories("${EXTERNAL_DIR}/asio/asio/include")
include_directories("${EXTERNAL_DIR}/libpqxx/include")
include_directories("${EXTERNAL_DIR}/range-v3/include")
include_directories("${EXTERNAL_DIR}/Catch2/single_include")
include_directories("${ZLIB_INCLUDE_DIRS}")
include_directories("${EXTERNAL_DIR}/libsodium/src/libsodium/include")
include_directories("${EXTERNAL_DIR}/xxHash")
include_directories("${EXTERNAL_DIR}/wyhash")
include_directories("${EXTERNAL_DIR}/robin-hood-hashing/src/include")
include_directories("${EXTERNAL_DIR}/entt/src")
include_directories("${EXTERNAL_DIR}/pcg-cpp/include")
include_directories("${EXTERNAL_DIR}/concurrentqueue")
include_directories("${EXTERNAL_DIR}/cereal/include")
include_directories("${EXTERNAL_DIR}/magic_enum/include")


target_link_libraries(ibh_server PUBLIC ${PQXX_LIBRARY} -lpq)
target_link_libraries(ibh_server PUBLIC -lpthread -lstdc++fs)
target_link_libraries(ibh_server PUBLIC ${ZLIB_LIBRARIES} )
target_link_libraries(ibh_server PUBLIC ${OPENSSL_LIBRARIES})
target_link_libraries(ibh_server PUBLIC ${SODIUM_LIBRARY})
target_link_libraries(ibh_server PUBLIC -ltbb)

target_link_libraries(ibh_server_test PUBLIC ${PQXX_LIBRARY} -lpq)
target_link_libraries(ibh_server_test PUBLIC -lpthread -lstdc++fs)
target_link_libraries(ibh_server_test PUBLIC ${ZLIB_LIBRARIES} )
target_link_libraries(ibh_server_test PUBLIC ${OPENSSL_LIBRARIES})
target_link_libraries(ibh_server_test PUBLIC ${SODIUM_LIBRARY})
target_link_libraries(ibh_server_test PUBLIC -ltbb)

target_link_libraries(ibh_benchmark PUBLIC ${PQXX_LIBRARY} -lpq)
target_link_libraries(ibh_benchmark PUBLIC -lpthread -lstdc++fs)
target_link_libraries(ibh_benchmark PUBLIC ${ZLIB_LIBRARIES} )
target_link_libraries(ibh_benchmark PUBLIC ${OPENSSL_LIBRARIES})
target_link_libraries(ibh_benchmark PUBLIC ${SODIUM_LIBRARY})
target_link_libraries(ibh_benchmark PUBLIC -ltbb)

add_custom_command(
        TARGET ibh_server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets/
        ${CMAKE_BINARY_DIR}/assets)

add_custom_command(
        TARGET ibh_server_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets/
        ${CMAKE_BINARY_DIR}/assets)
