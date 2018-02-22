
macro(nap_qt_pre)

    ## First, let cmake know where the Qt library path is, we go from there.
    if(MSVC OR APPLE)
        # Pick up QT_DIR environment variable
        if(DEFINED ENV{QT_DIR})
            set(QTDIR $ENV{QT_DIR})
            message(STATUS "Using QT_DIR environment variable: ${QTDIR}")
        endif()

        # Add possible Qt installation paths to the HINTS section
        # The version probably doesn't have to match exactly (5.8.? is probably fine)
        find_path(QT_DIR lib/cmake/Qt5/Qt5Config.cmake
                  HINTS
                  ${QTDIR}
                  ${NAP_ROOT}/../Qt/5.9.1/msvc2015_64
                  ${NAP_ROOT}/../Qt/5.9.2/msvc2015_64
                  ${NAP_ROOT}/../Qt/5.10.0/msvc2015_64
                  ~/Qt/5.8/clang_64
                  )
        # Find_package for Qt5 will pick up the Qt installation from CMAKE_PREFIX_PATH
        set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${QT_DIR})

        if(DEFINED QT_DIR)
            if(APPLE)
                  # Ensure we're not using Qt from homebrew as we don't know the legal situation with packaging homebrew's packages.
                  # Plus Qt's own opensource packages should have wider macOS version support.
                  if(EXISTS ${QT_DIR}/INSTALL_RECEIPT.json)
                      message(FATAL_ERROR "Homebrew's Qt packages aren't allowed due largely to a legal unknown.  Install Qt's own opensource release and point environment variable QT_DIR there.")
                  endif()

                  # TODO Ideally add stronger verification that ensures we're finding Qt's own package, or at least don't allow macports' Qt
            endif()
        else()
            message(WARNING
                    "The QT5 Directory could not be found, "
                    "consider setting the QT_DIR environment variable "
                    "to something like: \"C:/dev/Qt/5.9.1/msvc2015_64\"")
        endif()
    endif()


    find_package(Qt5Core REQUIRED)
    find_package(Qt5Widgets REQUIRED)
    find_package(Qt5Gui REQUIRED)

    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    add_definitions(-DQT_NO_KEYWORDS)

    set(QT_LIBS Qt5::Widgets Qt5::Core Qt5::Gui)

endmacro()


macro(nap_qt_post PROJECTNAME)
    qt5_use_modules(${PROJECT_NAME} Core Widgets Gui)

    if(WIN32)
        add_custom_command(TARGET ${PROJECTNAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
                           $<TARGET_FILE:Qt5::Widgets>
                           $<TARGET_FILE:Qt5::Core>
                           $<TARGET_FILE:Qt5::Gui>
                           $<TARGET_FILE_DIR:${PROJECTNAME}>
                           COMMENT "Copy Qt DLLs")
    endif()

    add_custom_command(TARGET ${PROJECTNAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_CURRENT_LIST_DIR}/resources
                       $<TARGET_FILE_DIR:${PROJECTNAME}>/resources
                       COMMENT "Copy Resources")
endmacro()