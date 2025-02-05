cmake_minimum_required(VERSION 3.18.4)
# Exclude for Android
if(ANDROID)
    return()
endif()

project(mod_napyoctopuce)

# add all cpp files to SOURCES
file(GLOB_RECURSE SOURCES src/*.cpp src/*.h)

# Get our NAP modules dependencies from module.json
module_json_to_cmake()

find_package(yoctopuce REQUIRED)

# LIBRARY

# compile shared lib as target
add_library(${PROJECT_NAME} SHARED ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Modules)
# Remove lib prefix on Unix libraries
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

# add include dirs
target_include_directories(${PROJECT_NAME} PUBLIC src ${YOCTO_INCLUDE_DIRS})

# preprocessor
target_compile_definitions(${PROJECT_NAME} PRIVATE NAP_SHARED_LIBRARY)

# link with external libs
if(NOT WIN32)
	target_compile_definitions(${PROJECT_NAME} PUBLIC HAVE_CONFIG_H)
endif()
target_link_libraries(${PROJECT_NAME} napcore ${DEPENDENT_NAP_MODULES} yoctopuce)

if (WIN32)
  copy_yoctopuce_dll()
endif()

# Deploy module.json as MODULENAME.json alongside module post-build
copy_module_json_to_bin()

# Package into platform release
if(APPLE)
    set(MACOS_EXTRA_RPATH_RELEASE ../../../../thirdparty/yoctopuce/bin)
    set(MACOS_EXTRA_RPATH_DEBUG ${MACOS_EXTRA_RPATH_RELEASE})    
elseif(UNIX)
    set(LINUX_EXTRA_RPATH ../../../../thirdparty/yoctopuce/bin)
endif()
package_module()

# Package yoctopuce into platform release
# TODO Package yoctopuce into platform release if we can find it
install(DIRECTORY ${THIRDPARTY_DIR}/yoctopuce/Sources/ 
        DESTINATION thirdparty/yoctopuce/include
        FILES_MATCHING PATTERN "*.h")
if(WIN32)
    install(FILES ${THIRDPARTY_DIR}/yoctopuce/Binaries/windows/x64/Release/yocto.dll DESTINATION thirdparty/yoctopuce/bin/release)
    install(FILES ${THIRDPARTY_DIR}/yoctopuce/Binaries/windows/x64/Release/yocto.lib DESTINATION thirdparty/yoctopuce/bin/release)
    install(FILES ${THIRDPARTY_DIR}/yoctopuce/Binaries/windows/x64/Debug/yocto.dll DESTINATION thirdparty/yoctopuce/bin/debug)
    install(FILES ${THIRDPARTY_DIR}/yoctopuce/Binaries/windows/x64/Debug/yocto.lib DESTINATION thirdparty/yoctopuce/bin/debug)
elseif(APPLE)
    SET(YOCTO_DYLIB ${THIRDPARTY_DIR}/yoctopuce/Binaries/osx/libyocto.dylib)
    install(FILES ${YOCTO_DYLIB} DESTINATION thirdparty/yoctopuce/bin)

    # Ensure our yoctopuce install name id is set properly, this is really for intall into packaging
    add_custom_command(TARGET ${PROJECT_NAME}
                       PRE_BUILD
                       COMMAND ${CMAKE_INSTALL_NAME_TOOL} -id @rpath/libyocto.dylib ${YOCTO_DYLIB}
                       COMMENT "Setting install name for yoctopuce")

    foreach(build_type Release Debug)
        install(CODE "execute_process(COMMAND ${CMAKE_INSTALL_NAME_TOOL}
                                              -add_rpath
                                              @loader_path/../../../../thirdparty/yoctopuce/bin
                                              ${CMAKE_INSTALL_PREFIX}/modules/mod_napyoctopuce/lib/${build_type}/mod_napyoctopuce.dylib
                                      ERROR_QUIET
                                      )")
    endforeach()    
else()
    install(FILES ${THIRDPARTY_DIR}/yoctopuce/Binaries/linux/64bits/libyocto.so.1.0.1 DESTINATION thirdparty/yoctopuce/bin)
endif()
