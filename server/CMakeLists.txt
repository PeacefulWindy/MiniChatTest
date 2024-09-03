cmake_minimum_required(VERSION 3.22)
project(server)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 23)

if(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Windows")
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        set(VCPKG_INSTALL_ROOT ${CMAKE_SOURCE_DIR}/out/build/x64-Debug/vcpkg_installed/${VCPKG_TARGET_TRIPLET})
    else()
        set(VCPKG_INSTALL_ROOT ${CMAKE_SOURCE_DIR}/out/build/x64-Release/vcpkg_installed/${VCPKG_TARGET_TRIPLET})
    endif()
    set(-D_WIN32_WINNT=0x0601)
else()
    set(VCPKG_INSTALL_ROOT ${CMAKE_SOURCE_DIR}/vcpkg_installed)
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    link_directories(${VCPKG_INSTALL_ROOT}/debug/lib)
else()
    link_directories(${VCPKG_INSTALL_ROOT}/lib)
endif()

include_directories(${CMAKE_SOURCE_DIR}/src)

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE})

aux_source_directory(src srcs)
aux_source_directory(src/lua srcs)
aux_source_directory(src/json srcs)
aux_source_directory(src/sql/mariadb srcs)
aux_source_directory(src/socket/session srcs)
aux_source_directory(src/socket/tcp srcs)
aux_source_directory(src/queue srcs)
aux_source_directory(src/msg srcs)
aux_source_directory(src/service srcs)

find_package(lua CONFIG REQUIRED)
find_package(libhv CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_executable(${PROJECT_NAME} ${srcs})

target_include_directories(${PROJECT_NAME} PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LUA_LIBRARIES} hv spdlog::spdlog_header_only lua cjson libmariadb)