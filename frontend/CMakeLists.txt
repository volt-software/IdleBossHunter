cmake_minimum_required(VERSION 3.13)
project(ibh_frontend)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE CXX STANDARD REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Debug CACHE STRING
            "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
            FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

option(EMSCRIPTEN "Compile webassembly version" OFF)
option(USE_SANITIZERS "Compile with sanitizers, slowing down the build and the runtime" OFF)

IF(EMSCRIPTEN)
    SET(CMAKE_CXX_COMPILER em++)
ENDIF()

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/frontend/bin)
set(TOP_DIR ${CMAKE_SOURCE_DIR})
set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/external/)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DXXH_INLINE_ALL -DXXH_CPU_LITTLE_ENDIAN=1 -DSPDLOG_COMPILED_LIB -DCATCH_CONFIG_FAST_COMPILE -DSPDLOG_NO_EXCEPTIONS -DIMGUI_DISABLE_OBSOLETE_FUNCTIONS -DENTT_NO_ETO=1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-variable -Wno-long-long -Wno-unused-parameter -Wnull-dereference -pedantic ")

IF(EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='[\"bmp\",\"png\",\"xpm\"]' -s USE_SDL_MIXER=2 -s USE_SDL_TTF=2 -s USE_OGG=1 -s USE_VORBIS=1 -msimd128 ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s ASSERTIONS=1 -s GL_ASSERTIONS=1 -s FULL_ES2=1 -s ALLOW_MEMORY_GROWTH=1 -s WEBSOCKET_DEBUG=0 --shell-file ${CMAKE_SOURCE_DIR}/frontend/src/emscripten_shell.html ")
ELSE()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DASIO_STANDALONE -DASIO_CONCURRENCY_HINT_DEFAULT=1")
ENDIF()


IF(USE_SANITIZERS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined -fno-omit-frame-pointer ") #-fsanitize=undefined -fsanitize=address -fsanitize=thread ")
ENDIF()

IF(EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS_DEBUG "-g4 --source-map-base https://www.realmofaesir.com/")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g4 -Os --source-map-base https://www.realmofaesir.com/")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")
ELSE()
    set(CMAKE_CXX_FLAGS_DEBUG "-g3 -ggdb -fno-stack-protector")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -ggdb -Og")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")
ENDIF()

file(GLOB_RECURSE PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE COMMON_SOURCES ${TOP_DIR}/common/src/*.cpp)
file(GLOB SPDLOG_SOURCE ${EXTERNAL_DIR}/spdlog/src/*.cpp)
file(GLOB ZstdCommonSources ${LIBRARY_DIR}/common/*.c)
file(GLOB ZstdCompressSources ${LIBRARY_DIR}/compress/*.c)
file(GLOB ZstdDecompressSources ${LIBRARY_DIR}/decompress/*.c)
file(GLOB ZstdDictBuilderSources ${LIBRARY_DIR}/dictBuilder/*.c)

IF(EMSCRIPTEN)
    add_executable(ibh_frontend.html ${SPDLOG_SOURCE} ${PROJECT_SOURCES} ${COMMON_SOURCES} ${ZstdCommonSources} ${ZstdCompressSources} ${ZstdDecompressSources} ${ZstdDictBuilderSources})
ELSE()
    add_executable(ibh_frontend ${SPDLOG_SOURCE} ${PROJECT_SOURCES} ${COMMON_SOURCES} ${ZstdCommonSources} ${ZstdCompressSources} ${ZstdDecompressSources} ${ZstdDictBuilderSources})
ENDIF()

file(GLOB_RECURSE PROJECT_SOURCES_WITHOUT_MAIN ${PROJECT_SOURCE_DIR}/src/*.cpp)
list(REMOVE_ITEM PROJECT_SOURCES_WITHOUT_MAIN "${PROJECT_SOURCE_DIR}/src/main.cpp")
file(GLOB_RECURSE PROJECT_TEST_SOURCES ${PROJECT_SOURCE_DIR}/test/*.cpp)

IF(EMSCRIPTEN)
    add_executable(ibh_frontend_test.html ${SPDLOG_SOURCE} ${PROJECT_SOURCES_WITHOUT_MAIN} ${COMMON_SOURCES} ${PROJECT_TEST_SOURCES})
    add_test(NAME MyTest COMMAND ibh_frontend_test.html)

    target_compile_definitions(ibh_frontend_test.html PRIVATE TEST_CODE=1)
ELSE()
    add_executable(ibh_frontend_test ${SPDLOG_SOURCE} ${PROJECT_SOURCES_WITHOUT_MAIN} ${COMMON_SOURCES} ${PROJECT_TEST_SOURCES})
    add_test(NAME MyTest COMMAND ibh_frontend_test)
ENDIF()


file(GLOB_RECURSE PROJECT_SOURCES_WITHOUT_MAIN ${PROJECT_SOURCE_DIR}/src/*.cpp)
list(REMOVE_ITEM PROJECT_SOURCES_WITHOUT_MAIN "${PROJECT_SOURCE_DIR}/src/main.cpp")
file(GLOB PROJECT_BENCHMARK_SOURCES ${PROJECT_SOURCE_DIR}/benchmark/*.cpp ${PROJECT_SOURCE_DIR}/benchmark/benchmark_helpers/*.cpp)

IF(EMSCRIPTEN)
    add_executable(ibh_frontend_benchmark.html ${SPDLOG_SOURCE} ${PROJECT_SOURCES_WITHOUT_MAIN} ${COMMON_SOURCES} ${PROJECT_BENCHMARK_SOURCES})
ELSE()
    add_executable(ibh_frontend_benchmark ${SPDLOG_SOURCE} ${PROJECT_SOURCES_WITHOUT_MAIN} ${COMMON_SOURCES} ${PROJECT_BENCHMARK_SOURCES})
ENDIF()

include_directories("${TOP_DIR}/common/src")
include_directories("${TOP_DIR}/frontend/src")
include_directories("${EXTERNAL_DIR}/glm")
include_directories("${EXTERNAL_DIR}/zstd/lib")
include_directories("${EXTERNAL_DIR}/zstd/lib/common")
include_directories("${EXTERNAL_DIR}/spdlog/include")
include_directories("${EXTERNAL_DIR}/rapidjson/include")
include_directories("${EXTERNAL_DIR}/range-v3/include")
include_directories("${EXTERNAL_DIR}/Catch2/single_include")
include_directories("${EXTERNAL_DIR}/xxHash")
include_directories("${EXTERNAL_DIR}/wyhash")
include_directories("${EXTERNAL_DIR}/robin-hood-hashing/src/include")
include_directories("${EXTERNAL_DIR}/entt/src")
include_directories("${EXTERNAL_DIR}/pcg-cpp/include")
include_directories("${EXTERNAL_DIR}/concurrentqueue")
include_directories("${EXTERNAL_DIR}/cereal/include")
include_directories("${EXTERNAL_DIR}/magic_enum/include")

IF(EMSCRIPTEN)
    set_target_properties(ibh_frontend.html PROPERTIES LINK_FLAGS "--no-heap-copy --preload-file ${TOP_DIR}/assets@/assets -s DISABLE_EXCEPTION_CATCHING=0")
    target_link_libraries(ibh_frontend.html PUBLIC -lpthread -lwebsocket.js)
    target_link_libraries(ibh_frontend.html PUBLIC ${ZLIB_LIBRARIES} )
    target_link_libraries(ibh_frontend.html PUBLIC ${OPENSSL_LIBRARIES})
    target_link_libraries(ibh_frontend.html PUBLIC -static-libgcc -static-libstdc++)

    target_link_libraries(ibh_frontend_test.html PUBLIC -lpthread -lwebsocket.js)
    target_link_libraries(ibh_frontend_test.html PUBLIC ${ZLIB_LIBRARIES} )
    target_link_libraries(ibh_frontend_test.html PUBLIC ${OPENSSL_LIBRARIES})
    target_link_libraries(ibh_frontend_test.html PUBLIC -static-libgcc -static-libstdc++)

    target_link_libraries(ibh_frontend_benchmark.html PUBLIC -lpthread -lwebsocket.js)
    target_link_libraries(ibh_frontend_benchmark.html PUBLIC ${ZLIB_LIBRARIES} )
    target_link_libraries(ibh_frontend_benchmark.html PUBLIC ${OPENSSL_LIBRARIES})
    target_link_libraries(ibh_frontend_benchmark.html PUBLIC -static-libgcc -static-libstdc++)

    add_custom_command(
            TARGET ibh_frontend.html POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets/
            ${CMAKE_BINARY_DIR}/assets)

    add_custom_command(
            TARGET ibh_frontend_test.html POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets/
            ${CMAKE_BINARY_DIR}/assets)
ELSE()
    include_directories("${EXTERNAL_DIR}/websocketpp")
    include_directories("${EXTERNAL_DIR}/asio/asio/include")
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake_modules")

    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_WIN32_THREADS_INIT 0)
    set(CMAKE_USE_PTHREADS_INIT 1)
    set(THREADS_PREFER_PTHREAD_FLAG ON)

    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    find_package(SDL2_mixer REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(ZLIB REQUIRED)
    find_package(OpenSSL REQUIRED)

    include_directories(${GLEW_INCLUDE_DIRS})
    include_directories(${SDL2_INCLUDE_DIRS})
    include_directories(${SDL2_IMAGE_INCLUDE_DIR})
    include_directories(${SDL2TTF_INCLUDE_DIR})
    include_directories(${SDL2Mixer_INCLUDE_DIR})
    include_directories(${OPENGL_INCLUDE_DIRS})

    target_link_libraries(ibh_frontend PUBLIC -lpthread -ltbb)
    target_link_libraries(ibh_frontend PUBLIC ${ZLIB_LIBRARIES} )
    target_link_libraries(ibh_frontend PUBLIC ${OPENSSL_LIBRARIES})
    target_link_libraries(ibh_frontend PUBLIC ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARY} ${SDL2TTF_LIBRARY} ${SDL2Mixer_LIBRARY})
    target_link_libraries(ibh_frontend PUBLIC ${GLEW_LIBRARIES})
    target_link_libraries(ibh_frontend PUBLIC ${OPENGL_LIBRARIES})
    target_link_libraries(ibh_frontend PUBLIC -static-libgcc -static-libstdc++)

    target_link_libraries(ibh_frontend_test PUBLIC -lpthread -ltbb)
    target_link_libraries(ibh_frontend_test PUBLIC ${ZLIB_LIBRARIES} )
    target_link_libraries(ibh_frontend_test PUBLIC ${OPENSSL_LIBRARIES})
    target_link_libraries(ibh_frontend_test PUBLIC ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARY} ${SDL2TTF_LIBRARY} ${SDL2Mixer_LIBRARY})
    target_link_libraries(ibh_frontend_test PUBLIC ${GLEW_LIBRARIES})
    target_link_libraries(ibh_frontend_test PUBLIC ${OPENGL_LIBRARIES})
    target_link_libraries(ibh_frontend_test PUBLIC -static-libgcc -static-libstdc++)

    target_link_libraries(ibh_frontend_benchmark PUBLIC -lpthread -ltbb)
    target_link_libraries(ibh_frontend_benchmark PUBLIC ${ZLIB_LIBRARIES} )
    target_link_libraries(ibh_frontend_benchmark PUBLIC ${OPENSSL_LIBRARIES})
    target_link_libraries(ibh_frontend_benchmark PUBLIC ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARY} ${SDL2TTF_LIBRARY} ${SDL2Mixer_LIBRARY})
    target_link_libraries(ibh_frontend_benchmark PUBLIC ${GLEW_LIBRARIES})
    target_link_libraries(ibh_frontend_test PUBLIC ${OPENGL_LIBRARIES})
    target_link_libraries(ibh_frontend_benchmark PUBLIC -static-libgcc -static-libstdc++)

    add_custom_command(
            TARGET ibh_frontend POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets/
            ${CMAKE_BINARY_DIR}/assets)

    add_custom_command(
            TARGET ibh_frontend_test POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets/
            ${CMAKE_BINARY_DIR}/assets)
ENDIF()

