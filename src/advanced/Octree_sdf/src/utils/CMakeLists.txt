add_library(utils INTERFACE)
target_include_directories(utils INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(Triangle3D_test Triangle3D_test.cpp)
target_link_libraries(Triangle3D_test PRIVATE utils)

add_executable(Posture_test Posture_test.cpp)
target_link_libraries(Posture_test PRIVATE utils)
