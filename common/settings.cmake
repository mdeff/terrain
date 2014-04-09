# Build configuration file for "Intro to Graphics"
# Copyright (C) 2014 - LGG EPFL
#
#--- To understand its content: 
#   http://www.cmake.org/cmake/help/syntax.html
#   http://www.cmake.org/Wiki/CMake_Useful_Variables
#

#--- This is how you show a status message in the build system
# MESSAGE(STATUS "Intro to Graphics - Loading Common Configuration")

#--- Tell CMake he'll be able to include ".cmake" configurations 
# files in the folder where the current file is located
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

#--- CMake extension to load GLFW
find_package(GLFW REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})
add_definitions(${GLFW_DEFINITIONS})
if(NOT GLFW_FOUND)
    message(ERROR " GLFW not found!")
endif()

#--- GLEW (STATICALLY LINKED)
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
link_directories(${GLEW_LIBRARY_DIRS})
add_definitions(-DGLEW_STATIC)
# message(STATUS "HERE " ${GLEW_LIBRARIES})

#--- stringify shader macro
macro(target_stringify_shader MYTARGET SHADER)
    set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    set(TARGET_DIR ${CMAKE_CURRENT_BINARY_DIR})
    set(TARGET_NAME stringify_${MYTARGET}_${SHADER})
    add_custom_command(
        # The command produces a shader.h file in output
        OUTPUT ${SHADER}.h 
        COMMAND ${CMAKE_COMMAND} 
        # Parameters of the script
        -DSHADERNAME=${SHADER}
        -DINPUT_DIR=${SOURCE_DIR}
        -DOUTPUT_DIR=${TARGET_DIR}
        # Executes the script
        -P ${CMAKE_SOURCE_DIR}/common/stringify_shader.cmake
        # And when it does spit out this command
        DEPENDS ${SOURCE_DIR}/${SHADER}.glsl
        COMMENT "Converting ${SHADER}.glsl to a char* in ${SHADER}.h")
    # Creates dummy target (to add dependencies)
    add_custom_target(${TARGET_NAME} DEPENDS ${SHADER}.h)
    # Creates dependency 
    add_dependencies(${MYTARGET} ${TARGET_NAME})
    # Make shader.h visible to the compile time (NEEDS MACRO!!)
    include_directories(${TARGET_DIR})
endmacro(target_stringify_shader)

#--- Wraps file deployments
macro(target_deploy_file MYTARGET FILEPATH)
    get_filename_component(FILENAME ${FILEPATH} NAME)
    set(TARGET_NAME deployfile_${MYTARGET}_${FILENAME})
    set(TARGET_DIR ${CMAKE_CURRENT_BINARY_DIR})
    add_custom_command(
        # Execute command after target is built (pre-run)
        TARGET ${MYTARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${FILEPATH} ${TARGET_DIR}
        DEPENDS ${FILEPATH}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copying ${FILEPATH} to ${TARGET_DIR}")
    add_dependencies(${MYTARGET} ${TARGET_NAME})
endmacro()

#--- FIND OPENGL
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND) 
    message(ERROR " OPENGL not found!")
endif()

#--- On UNIX|APPLE you can do "make update_opengp" to checkout a new fresh copy 
if(CMAKE_HOST_UNIX)
    add_custom_target( update_opengp
        COMMAND rm -rf OpenGP
        COMMAND svn checkout https://github.com/OpenGeometry/OpenGP/trunk/src/OpenGP
        COMMAND rm -rf OpenGP/.svn
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/external/OpenGP/include
        COMMENT "Updating OpenGP ${CMAKE_SOURCE_DIR}")
endif()

#--- FIND OPENGP
find_package(OpenGP REQUIRED)
include_directories(${OpenGP_INCLUDE_DIRS})
add_definitions(-DHEADERONLY)
add_definitions(-DUSE_EIGEN)
if(NOT OPENGP_FOUND)
    message(ERROR " OPENGP not found!")
endif()

#--- FIND EIGEN3
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIRS})
if(NOT EIGEN3_FOUND)
    message(ERROR " EIGEN not found!")
endif() 


#--- Common headers/libraries for all the exercises
include_directories(${CMAKE_CURRENT_LIST_DIR})
SET(COMMON_LIBS ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLEW_LIBRARIES})

# Strip all the solutions from any glsl/cpp file underneath a ex1/2/3..etc folder
# 
# USAGE: "cmake -DSTRIP_CODE=TRUE ../"
if(STRIP_CODE)
    file(GLOB_RECURSE SHRS "ex[0-9]/*.glsl")
    file(GLOB_RECURSE SRCS "ex[0-9]/*.cpp")
    set(FILES ${SHRS} ${SRCS})
    foreach(filename ${FILES})
        message(STATUS "stripping file: " ${filename})
        # unifdef -DSTRIP_CODE filename
        execute_process(
            COMMAND unifdef -DSTRIP_CODE
            -o ${filename} ${filename})
    endforeach()
endif()

#--- OBSOLETE: as now shaders are deployed in .h files
# Each exercise outputs a binary to the source folder of each exercise
# otherwise, you'd have to move the shaders to the deploy folder!!!!!!
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
#--- OBSOLETE: use global deploy folder
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/deploy)

#--- TODO: CMake extension to specify launch configs for IDEs
# include(CreateLaunchers)
# create_target_launcher(ex2 WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
