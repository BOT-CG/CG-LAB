add_library(SDFOctree INTERFACE)
target_link_libraries(SDFOctree INTERFACE utils)
target_include_directories(SDFOctree INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
