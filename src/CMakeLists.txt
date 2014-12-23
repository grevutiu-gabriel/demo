cmake_minimum_required(VERSION 2.8.8)
project("demo")

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(cotire)

#generates qt moc and ui sources from the list of source files which have been given as parameter
MACRO (generateQtOutputs sources )
        #
        # ui output
        #
        set( ui_files )
        #for each source
        FOREACH (src ${${sources}})
                # filter ui files
                if(${src} MATCHES "\\.ui$")
                        set( ui_files ${ui_files} ${src} )
                endif(${src} MATCHES "\\.ui$")
        ENDFOREACH (src)

        # generate moc outputs
        QT5_WRAP_UI( ui_outputs ${ui_files})

        # append outputs to sources
        set( ${sources} ${${sources}} ${ui_files} )

        # put generated files into a sourcegroup called generated
        #source_group(generated FILES ${${ui_outputs}})
ENDMACRO (generateQtOutputs)

# binaries will go to seperate bin directory one level above
#if( $ENV{USERNAME} STREQUAL "koernedd" ) #optional: different behavior per user
get_filename_component(bin_directory ${CMAKE_CURRENT_SOURCE_DIR}/../bin ABSOLUTE)
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${bin_directory} )
#endif( $ENV{USERNAME} STREQUAL "koernedd" )
add_definitions(-DSRC_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}\")
add_definitions(-DDATA_PATH=\"${bin_directory}/data\")



set( APP_NAME "demo" )
MESSAGE( STATUS "application:" ${APP_NAME} )
MESSAGE( STATUS "binary output:" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )

# on windows
if( CMAKE_SYSTEM_NAME MATCHES Windows )
    add_definitions(-DNOMINMAX) # windows defines min and max in winbase.h which clashes with stl
endif( CMAKE_SYSTEM_NAME MATCHES Windows )


# QT 5.0 -------------------------------------------
# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Widgets finds its own dependencies.
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)

# Oculus Rift ----------------------------------
#add_subdirectory( 3rdparty/LibOVR )
#include_directories( ${LIBOVR_INC_DIR} )

# PortAudio ------------------------------------
add_definitions(-DPA_USE_DS) # portaudio to use directx sound on windows
add_definitions(-DPA_WDMKS_NO_KSGUID_LIB) # https://groups.google.com/forum/#!topic/extemporelang/mJRnrBAdo0A

# sources ==========================================

# specify app sources
#file(GLOB_RECURSE app_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp ${CMAKE_CURRENT_SOURCE_DIR}/*.c ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.ui )
file(GLOB_RECURSE app_sources ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
							${CMAKE_CURRENT_SOURCE_DIR}/core/*
							${CMAKE_CURRENT_SOURCE_DIR}/framework/*
							${CMAKE_CURRENT_SOURCE_DIR}/gui/*
							${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/stblib/* )
							#${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/portaudio/* )
file(GLOB_RECURSE app_sources_glsl ${CMAKE_CURRENT_SOURCE_DIR}/*.glsl )

# add qt intermediate source files
generateQtOutputs( app_sources )


# executeable =====================================
add_executable(${APP_NAME} ${app_sources} ${app_sources_glsl})
#cotire(${APP_NAME})


# additional include directories ---
include_directories( ${BASE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/core )
include_directories( ${BASE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty )

# Qt ---
qt5_use_modules(${APP_NAME} Widgets OpenGL)

# Oculus Rift ------
#target_link_libraries( ${APP_NAME} LibOVR )

# OpenGL ---
INCLUDE(FindOpenGL)
target_link_libraries(${APP_NAME} ${OPENGL_gl_LIBRARY})
