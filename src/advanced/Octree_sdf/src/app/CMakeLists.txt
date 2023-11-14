find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::OpenGLWidgets)

target_link_libraries(${PROJECT_NAME} PRIVATE UI)

set_target_properties(${PROJECT_NAME} PROPERTIES
    # WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

###########################################################
add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND windeployqt "$<TARGET_FILE:${PROJECT_NAME}>"
--$<LOWER_CASE:$<CONFIG>>
)

set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION /)
include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME})
set(CPACK_NSIS_MENU_LINKS
${PROJECT_NAME}.exe ${PROJECT_NAME}
)
set(CPACK_GENERATOR NSIS)
set(CPACK_PACKAGE_VERSION_MAJOR "${sdf_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${sdf_VERSION_MINOR}")
include(CPack)
