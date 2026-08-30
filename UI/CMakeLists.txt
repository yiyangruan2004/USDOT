cmake_minimum_required(VERSION 3.16)
#"C:\Qt\6.11.1\msvc2022_64\bin\windeployqt6.exe" "C:\Users\yiyan\Desktop\USDOT\UI\build\Desktop_Qt_6_11_1_MSVC2022_64bit-Release\UI.exe"
project(UI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Charts)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Charts)

if(NOT EMSCRIPTEN)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Network)
endif()

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    logger.cpp
    logger.h
    ctrl.cpp
    ctrl.h
)

if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_add_executable(UI
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        monitor.h monitor.cpp

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET UI APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(UI SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(UI
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_compile_features(UI PRIVATE cxx_std_17)

target_link_libraries(UI PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Charts
)

if(NOT EMSCRIPTEN)
    target_link_libraries(UI PRIVATE Qt${QT_VERSION_MAJOR}::Network)
endif()

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.UI)
endif()
set_target_properties(UI PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS UI
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(UI)
endif()

include(CTest)

if(BUILD_TESTING AND NOT EMSCRIPTEN)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

    add_executable(UI_webdemo_tests
        webdemo_test.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        ctrl.cpp
        ctrl.h
        monitor.cpp
        monitor.h
    )

    target_compile_features(UI_webdemo_tests PRIVATE cxx_std_17)
    target_link_libraries(UI_webdemo_tests PRIVATE
        Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Charts
        Qt${QT_VERSION_MAJOR}::Network
    )

    add_test(NAME UI_webdemo_tests COMMAND UI_webdemo_tests)
endif()
