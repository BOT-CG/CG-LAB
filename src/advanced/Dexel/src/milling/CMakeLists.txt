add_library(milling STATIC
    Cutters/CutterAPT.cpp
    Cutters/CutterBallEndmill.cpp
    Cutters/CutterFilletEndmill.cpp
    Cutters/CutterFilletTaperEndmill.cpp
    Cutters/CutterFlatEndmill.cpp
    Cutters/CutterTaperEndmill.cpp
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
    FileStorage
    GLCompute
    DexelMilling
)

# # 链接glm数学库
# target_link_libraries(milling  PUBLIC glm::glm)
target_include_directories(milling PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(milling PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Cutters)
target_include_directories(milling PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/NCProgramStates)
