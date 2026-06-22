cmake_minimum_required(VERSION 3.19)
project(FukingLogger_ST_win_mingw64_qt6_8 LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(FukingLogger_ST_win_mingw64_qt6_8
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    logentry.h
    logparser.h
    logparser.cpp
    syntaxhighlighter.h
    syntaxhighlighter.cpp
    .gitignore
    fileloader.h
    fileloader.cpp
    logtab.h
    logtab.cpp
    filesplitter.h
    filesplitter.cpp
    linenumberarea.h
    linenumberarea.cpp
    settingsdialog.h
    settingsdialog.cpp
    styles.h
)

target_link_libraries(FukingLogger_ST_win_mingw64_qt6_8
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS FukingLogger_ST_win_mingw64_qt6_8
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET FukingLogger_ST_win_mingw64_qt6_8
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
