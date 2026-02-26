cmake_minimum_required(VERSION 3.16)

project(Remote VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

find_package(Qt6 REQUIRED COMPONENTS Quick Bluetooth)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appRemote
    src/main.cpp
    src/drone.h     src/drone.cpp
    src/udpdrone.h  src/udpdrone.cpp
    src/btledrone.h src/btledrone.cpp
    src/btdrone.h   src/btdrone.cpp
)

qt_add_qml_module(appRemote
    URI Remote
    QML_FILES
        qml/Main.qml
        QML_FILES qml/DebugControls.qml
        QML_FILES qml/Joystick.qml
        SOURCES src/backend.h src/backend.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appRemote PROPERTIES
    QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/android"
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appRemote
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appRemote
    PRIVATE Qt6::Quick Qt6::Bluetooth
)

include(GNUInstallDirs)
install(TARGETS appRemote
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
