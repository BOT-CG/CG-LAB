add_library(milling STATIC
    Cutters/CutterAPT.cpp
    Cutters/CutterBallEndmill.cpp
    Cutters/CutterFilletEndmill.cpp
    Cutters/CutterFilletTaperEndmill.cpp
    Cutters/CutterFlatEndmill.cpp
    Cutters/CutterStrange.cpp
    Cutters/CutterTaperEndmill.cpp
    Cutters/CutterTaperFilletEndmill.cpp
    NCProgramStates/NCProgramStateG0.cpp
    NCProgramStates/NCProgramStateG01.cpp
    NCProgramStates/NCProgramStateG02.cpp
    NCProgramStates/NCProgramStateG03.cpp
    NCProgramStates/NCProgramStateNone.cpp
    Cutter.cpp
    CutterFactory.cpp
    CutterSweptVolume.cpp
    GeometryMilling.cpp
    NCProgram.cpp
    Workpiece.cpp
)

if(LINUX OR MINGW)
    target_link_libraries(milling PRIVATE TBB::tbb)
endif()

target_link_libraries(milling PUBLIC
    utils
    SDFOctree
    FileStorage
    GLCompute
)

target_include_directories(milling PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Cutters)
target_include_directories(milling PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/NCProgramStates)
target_include_directories(milling PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(CutterAPT_test Cutters/CutterAPT_test.cpp)
target_link_libraries(CutterAPT_test PRIVATE milling)

add_executable(CutterFilletTaperEndmill_test Cutters/CutterFilletTaperEndmill_test.cpp)
target_link_libraries(CutterFilletTaperEndmill_test PRIVATE milling)
