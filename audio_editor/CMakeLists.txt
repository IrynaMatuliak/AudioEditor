cmake_minimum_required(VERSION 3.16)
project(audio_editor VERSION 0.1 LANGUAGES C CXX)

enable_testing()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Multimedia)  # For Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets Gui)

#add_library(qcustomplot STATIC ${CMAKE_SOURCE_DIR}/qcustomplot.cpp)

# Налаштування kissfft
set(KISSFFT_DIR "${CMAKE_SOURCE_DIR}/kiss_fft")
file(GLOB KISSFFT_SOURCES "${KISSFFT_DIR}/*.c")

add_library(kissfft STATIC ${KISSFFT_SOURCES})
target_include_directories(kissfft PUBLIC ${KISSFFT_DIR})

set(LIBS_DIR "${CMAKE_SOURCE_DIR}/libs")

set(SNDFILE_INCLUDE_DIR "${LIBS_DIR}/libsndfile/include")
set(SNDFILE_LIB_DIR "${LIBS_DIR}/libsndfile/lib")

if(WIN32)
    set(SNDFILE_LIBRARY "${SNDFILE_LIB_DIR}/sndfile.lib")  # Для Windows
else()
    set(SNDFILE_LIBRARY "${SNDFILE_LIB_DIR}/libsndfile.so") # Для Linux
endif()

set(LAME_INCLUDE_DIR "${LIBS_DIR}/lame/include")
set(LAME_LIB_DIR "${LIBS_DIR}/lame/lib")

if(WIN32)
    set(LAME_LIBRARY "${LAME_LIB_DIR}/libmp3lame.lib")  # Для Windows
else()
    set(LAME_LIBRARY "${LAME_LIB_DIR}/libmp3lame.so") # Для Linux
endif()

if(NOT EXISTS ${SNDFILE_LIBRARY})
    message(FATAL_ERROR "libsndfile library not found at ${SNDFILE_LIBRARY}")
endif()

if(NOT EXISTS ${LAME_LIBRARY})
    message(FATAL_ERROR "LAME library not found at ${LAME_LIBRARY}")
endif()

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
    createprojectwindow.cpp createprojectwindow.h createprojectwindow.ui
    projectworkwindow.cpp projectworkwindow.h projectworkwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(audio_editor
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        resources.qrc
        waveformwidget.h waveformwidget.cpp
        menubarmanager.h menubarmanager.cpp
        helpwindow.h helpwindow.cpp helpwindow.ui
        echodialog.h echodialog.cpp
        echodialog.h echodialog.cpp echodialog.ui
        equalizerdialog.h equalizerdialog.cpp equalizerdialog.ui
        noisereductiondialog.h noisereductiondialog.cpp noisereductiondialog.ui
        fadedialog.h fadedialog.cpp fadedialog.ui

        createprojectwindow.h createprojectwindow.cpp createprojectwindow.ui
        createprojectwindow.h createprojectwindow.cpp createprojectwindow.ui



    )
else()
    add_executable(audio_editor ${PROJECT_SOURCES})
endif()

target_link_libraries(audio_editor PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

target_link_libraries(audio_editor PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Multimedia
    ${LAME_LIBRARY}
    ${SNDFILE_LIBRARY}
    kissfft
    AudioEffects
)

target_include_directories(audio_editor PRIVATE
    ${SNDFILE_INCLUDE_DIR}
    ${LAME_INCLUDE_DIR}
    ${KISSFFT_DIR}
)

if(WIN32)
    add_custom_command(TARGET audio_editor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            "${LIBS_DIR}/libsndfile/bin/sndfile.dll"
            $<TARGET_FILE_DIR:audio_editor>
        COMMAND ${CMAKE_COMMAND} -E copy
            "${LIBS_DIR}/lame/bin/lame_enc.dll"
            $<TARGET_FILE_DIR:audio_editor>
    )
endif()

set_target_properties(audio_editor PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(audio_editor)
endif()
add_subdirectory(tests)
add_subdirectory(AudioEffects)
