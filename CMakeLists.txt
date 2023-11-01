cmake_minimum_required(VERSION 3.10)
project(OpenGLTest)

# Integrate with vcpkg
# Use environment variables or CMake options for portability
set(VCPKG_ROOT E:/code/cpp/opengltest/vcpkg/)

set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake CACHE STRING "")
set(CMAKE_PREFIX_PATH ${VCPKG_ROOT}/installed/x64-mingw-static/share CACHE STRING "")

# Finding required packages
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)

# Shared utility library
file(GLOB SHARED_UTILS_SRC "src/utils/*.cpp" "src/utils/*.h")
add_library(utils ${SHARED_UTILS_SRC})
target_link_libraries(utils PRIVATE glad::glad glfw glm::glm assimp::assimp)
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/utils)

# Levels and subprojects
set(LEVELS basic medium) # ... add more levels as needed
set(BASIC_SUBPROJECTS hello_window 1.1draw_triangle 1.2factory 1.3draw_rectangle 1.4shader 1.5texture 1.6camera )
set(MEDIUM_SUBPROJECTS 2.1ambientLight 2.2diffuseLight 2.3objLoader 2.4lightWithModel 2.5gimbalLockCamera 2.6bumpLighting 2.7quaternionCamera)

foreach(level IN LISTS LEVELS)
    message(STATUS "Level: ${level}")

    # Access the correct subproject list
    string(TOUPPER ${level} LEVEL_UPPERCASE)
    set(CURRENT_SUBPROJECTS ${${LEVEL_UPPERCASE}_SUBPROJECTS})

    foreach(subproject IN LISTS CURRENT_SUBPROJECTS)
        message(STATUS "  Subproject: ${subproject}")

        # Optional utility sources specific to subproject
        file(GLOB UTILS_SRC "src/${level}/${subproject}/utils/*.cpp" "src/${level}/${subproject}/utils/*.h")

        # Main executable
        add_executable(${subproject} "src/${level}/${subproject}/main.cpp" ${UTILS_SRC})
        target_link_libraries(${subproject} PRIVATE utils glad::glad glfw glm::glm assimp::assimp yaml-cpp)

        set_target_properties(${subproject} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${level}/${subproject}")

        # Copy dependencies to the output directory (if any)
        set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/${level}/${subproject}/dependencies")

        if(EXISTS ${SOURCE_DIR})
            add_custom_command(TARGET ${subproject} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${SOURCE_DIR} $<TARGET_FILE_DIR:${subproject}>
            )
        endif()
    endforeach()
endforeach()
