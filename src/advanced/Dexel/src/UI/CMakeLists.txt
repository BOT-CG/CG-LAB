find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets Charts LinguistTools)

add_library(UI STATIC
    ChartMRR.cpp
    DialogAddCutter.cpp
    DialogAddNCProgramManual.cpp
    DialogAddWorkpieceManual.cpp
    DialogAutodiff.cpp
    DialogSettings.cpp
    DockWidget.cpp
    GLWidget.cpp
    GLWidgetCutter.cpp
    ListViewCutter.cpp
    ListViewNCProgram.cpp
    MainWindow.cpp
    MenuBar.cpp
    ObjectLanguage.cpp
    ObjectMilling.cpp
    Settings.cpp
    StatusBar.cpp
    TabWidgetProcedure.cpp
    ToolBarAnimation.cpp
    ToolBarCreation.cpp
    ToolBarOthers.cpp
    ToolBarVisible.cpp
    icons.qrc
    images.qrc
    ncs.qrc
    stls.qrc
    translations.qrc
)

set(TS_FILES
    translations/i18n_en.ts
    translations/i18n_zh.ts
)

set_source_files_properties(
    ${TS_FILES}
    PROPERTIES OUTPUT_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/translations"
)

qt_add_translations(UI TS_FILES
    ${TS_FILES}
    QM_FILES_OUTPUT_VARIABLE qm_files
)

target_compile_definitions(UI PRIVATE QT_NO_KEYWORDS)

target_link_libraries(UI PRIVATE
    Qt6::Widgets
    Qt6::OpenGLWidgets
    Qt6::Charts
)

target_link_libraries(UI PRIVATE
    utils
    FileStorage
    render
    milling
)

target_include_directories(UI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
