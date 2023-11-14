find_package(Qt6 REQUIRED COMPONENTS OpenGL)

add_library(GLCompute STATIC
    GLCompute.cpp
    computeShaders.qrc
)

target_link_libraries(GLCompute PRIVATE Qt6::OpenGL)

target_link_libraries(GLCompute PUBLIC utils)

target_include_directories(GLCompute PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
