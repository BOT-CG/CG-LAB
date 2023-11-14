find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS OpenGL)

add_library(render STATIC
    Camera.cpp
    Render.cpp
    RenderAutodiff.cpp
    RenderCutter.cpp
    RenderCWE.cpp
    shaders.qrc
)

target_link_libraries(render PRIVATE Qt6::Widgets)
target_link_libraries(render PRIVATE Qt6::OpenGL)

target_link_libraries(render PUBLIC
    utils
)

target_include_directories(render PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
