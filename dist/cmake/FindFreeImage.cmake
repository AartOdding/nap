if (WIN32)
    find_path(
        FREEIMAGE_DIR
        NAMES bin/FreeImage.dll
        HINTS ${CMAKE_CURRENT_LIST_DIR}/../thirdparty/FreeImage
    )
    set(FREEIMAGE_INCLUDE_DIRS ${FREEIMAGE_DIR}/include/)
    set(FREEIMAGE_LIBS_DLL ${FREEIMAGE_DIR}/bin/FreeImage.dll)
    set(FREEIMAGE_IMPLIB ${FREEIMAGE_DIR}/lib/FreeImage.lib)
elseif(APPLE)
    find_path(
        FREEIMAGE_DIR
        NAMES include/FreeImage.h
        HINTS ${CMAKE_CURRENT_LIST_DIR}/../thirdparty/FreeImage
    )
    set(FREEIMAGE_INCLUDE_DIRS ${FREEIMAGE_DIR}/include/)
endif()

mark_as_advanced(FREEIMAGE_INCLUDE_DIRS)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(freeimage REQUIRED_VARS FREEIMAGE_INCLUDE_DIRS)

if (WIN32)
    add_library(freeimage SHARED IMPORTED)
    set_target_properties(freeimage PROPERTIES
      IMPORTED_CONFIGURATIONS "Debug;Release;MinSizeRel;RelWithDebInfo"
      IMPORTED_LOCATION_RELEASE ${FREEIMAGE_LIBS_DLL}
      IMPORTED_LOCATION_DEBUG ${FREEIMAGE_LIBS_DLL}
      IMPORTED_LOCATION_MINSIZEREL ${FREEIMAGE_LIBS_DLL}
      IMPORTED_LOCATION_RELWITHDEBINFO ${FREEIMAGE_LIBS_DLL}
      IMPORTED_IMPLIB ${FREEIMAGE_IMPLIB}
  )
endif()
