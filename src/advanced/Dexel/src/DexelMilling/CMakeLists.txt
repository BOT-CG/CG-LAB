find_package(glm CONFIG REQUIRED)

add_library(DexelMilling STATIC
    DexelMilling.cpp
)
target_link_libraries(DexelMilling PUBLIC utils)
target_link_libraries(DexelMilling PUBLIC glm::glm)

# target_link_libraries(DexelMilling PUBLIC SDFOctree)
target_link_libraries(DexelMilling PUBLIC milling)
target_include_directories(DexelMilling PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})