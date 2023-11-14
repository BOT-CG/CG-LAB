add_library(FileStorage STATIC STLFile.cpp)
target_link_libraries(FileStorage PUBLIC utils)
target_include_directories(FileStorage PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(STLFile_test STLFile_test.cpp)
target_link_libraries(STLFile_test PRIVATE FileStorage)
