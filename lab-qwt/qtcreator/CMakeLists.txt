cmake_minimum_required(VERSION 3.10)

project(lab-qwt VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Пути для Qwt
if(WIN32)
    set(QWT_ROOT "C:/qwt")  # Укажи свой путь
else()
    set(QWT_ROOT "/usr/local/qwt-6.3.0")  # Укажи свой путь
endif()

# Ищем Qwt
find_library(QWT_LIBRARY
    NAMES qwt qwt-qt5 qwt-qt6
    PATHS ${QWT_ROOT}/lib
    NO_DEFAULT_PATH
)

find_path(QWT_INCLUDE_DIR
    NAMES qwt_plot.h
    PATHS ${QWT_ROOT}/include
    NO_DEFAULT_PATH
)

# Проверяем, найдены ли Qwt
if (NOT QWT_LIBRARY OR NOT QWT_INCLUDE_DIR)
    message(FATAL_ERROR "Qwt not found! Please install Qwt and specify its location.")
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

set(PROJECT_SOURCES
        main.cpp


)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(lab-qwt
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET lab-qwt APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(lab-qwt SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(lab-qwt
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(lab-qwt PRIVATE ${QWT_INCLUDE_DIR})
target_link_libraries(lab-qwt PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network ${QWT_LIBRARY})

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.lab-qwt)
endif()
set_target_properties(lab-qwt PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS lab-qwt
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(lab-qwt)
endif()
