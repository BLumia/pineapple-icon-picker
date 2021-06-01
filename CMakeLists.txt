cmake_minimum_required(VERSION 3.5)

project(pineapple-icon-picker LANGUAGES CXX)

include (GNUInstallDirs)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Widgets Svg Xml LinguistTools REQUIRED)

set (PICONPICKER_CPP_FILES
    main.cpp
    mainwindow.cpp
    iconmodel.cpp
)

set (PICONPICKER_HEADER_FILES
    mainwindow.h
    iconmodel.h
)

set (PICONPICKER_UI_FILES
    mainwindow.ui
)

set (EXE_NAME pmusic)

# Translation
#file (GLOB PICONPICKER_TS_FILES languages/*.ts)
#set (PICONPICKER_CPP_FILES_FOR_I18N ${PMUSIC_CPP_FILES} ${PICONPICKER_TS_FILES})

#qt5_create_translation(PICONPICKER_QM_FILES ${PICONPICKER_CPP_FILES_FOR_I18N} ${PICONPICKER_TS_FILES})

add_executable(${EXE_NAME}
    ${PICONPICKER_HEADER_FILES}
    ${PICONPICKER_CPP_FILES}
    ${PICONPICKER_UI_FILES}

#    ${PICONPICKER_QM_FILES}
)

target_link_libraries(${EXE_NAME} PRIVATE Qt::Widgets Qt::Xml Qt::Svg)

# Extra build settings
if (WIN32)
    set_property (
        TARGET ${EXE_NAME}
        PROPERTY WIN32_EXECUTABLE true
    )
endif ()

# Helper macros for install settings
macro (pmusic_convert_to_relative_path _var)
    # Make sure _var is a relative path
    if (IS_ABSOLUTE "${${_var}}")
        file (RELATIVE_PATH ${_var} "${CMAKE_INSTALL_PREFIX}" "${${_var}}")
    endif ()
endmacro ()

# Install settings
if (WIN32)
    # FIXME: try to avoid install to a "bin" subfolder under windows...
    #        when fixed, don't forget to update the CI config file...
    set (BIN_INSTALL_DIR "") # seems useless, don't know why...
elseif (UNIX)
    if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        set(CMAKE_INSTALL_PREFIX /usr)
    endif ()

    set (BIN_INSTALL_DIR "${CMAKE_INSTALL_BINDIR}") # relative, usually "bin"
    pmusic_convert_to_relative_path(BIN_INSTALL_DIR)
    set (LIB_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}") # "lib" or "lib64"
    pmusic_convert_to_relative_path(LIB_INSTALL_DIR)

    # install icon
    install (
        FILES icons/app-icon.svg
        DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/48x48/apps"
        RENAME pineapple-icon-picker.svg
    )

    # install shortcut
    install (
        FILES pineapple-icon-picker.desktop
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications"
    )
endif()

set (INSTALL_TARGETS_DEFAULT_ARGS
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR} COMPONENT Devel
)

install (
    TARGETS ${EXE_NAME}
    ${INSTALL_TARGETS_DEFAULT_ARGS}
)

if (WIN32)
    set (QM_FILE_INSTALL_DIR "${CMAKE_INSTALL_BINDIR}/translations")
else ()
    set (QM_FILE_INSTALL_DIR "${CMAKE_INSTALL_FULL_DATADIR}/pineapple-music/translations")
    target_compile_definitions(${EXE_NAME}
        PRIVATE QM_FILE_INSTALL_DIR=${QM_FILE_INSTALL_DIR}
    )
endif ()

install (
    FILES ${PMUSIC_QM_FILES}
    DESTINATION ${QM_FILE_INSTALL_DIR}
)
