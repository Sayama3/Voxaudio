cmake_minimum_required(VERSION 3.23)

project(Voxaudio VERSION 0.0.1 DESCRIPTION "A multi backend audio engine.")

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_COMPUTED_DEFAULT "20")

option(DONT_ADD_GLM "The library will assume a target 'glm' already exist." OFF)
option(DONT_ADD_YAML-CPP "The library will assume a target 'yaml-cpp::yaml-cpp' already exist." OFF)
option(USE_FMOD_CORE_BACKEND "Use Fmod Core for the backend." OFF)
option(USE_FMOD_STUDIO_BACKEND "Use Fmod Studio for the backend." ON)

if(USE_FMOD_CORE_BACKEND OR USE_FMOD_STUDIO_BACKEND)
    add_subdirectory(lib/fmod)
endif()

if(NOT DONT_ADD_GLM)
    add_subdirectory(lib/glm)
endif()
if(NOT DONT_ADD_YAML)
    add_subdirectory(lib/yaml-cpp)
endif()

set(GLOBAL_SRC_FILES
    "include/Voxaudio.hpp"
    "Global/FileDialogs.cpp"
    "Global/FileDialogs.hpp"
    "Global/portable-file-dialogs.h"
)

set(FMOD_STUDIO_SRC_FILES
    "FmodStudio/FmodStudioVoxaudio.cpp"
    "FmodStudio/FmodStudioEngine.hpp"
    "FmodStudio/FmodStudioEngine.cpp"
)

set(FMOD_CORE_SRC_FILES 
    "FmodCore/FmodCoreVoxaudio.cpp"
    "FmodCore/FmodCoreEngine.hpp"
    "FmodCore/FmodCoreEngine.cpp"
)

if(USE_FMOD_STUDIO_BACKEND)
    set(SRC_FILES ${GLOBAL_SRC_FILES} ${FMOD_STUDIO_SRC_FILES})
elseif(USE_FMOD_CORE_BACKEND)
    set(SRC_FILES ${GLOBAL_SRC_FILES} ${FMOD_CORE_SRC_FILES})
else()
    message(FATAL_ERROR "No backend selected...")
endif()

add_library(Voxaudio ${SRC_FILES})


set_target_properties(Voxaudio PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(Voxaudio PROPERTIES SOVERSION 0)

set_target_properties(Voxaudio PROPERTIES
        VERSION       ${${PROJECT_NAME}_VERSION}
        PUBLIC_HEADER "include/Voxaudio.hpp"
        PUBLIC cxx_std_20
)

target_include_directories(Voxaudio PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_include_directories(Voxaudio PRIVATE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Global>"
        "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>"
)

if(USE_FMOD_STUDIO_BACKEND)
    target_link_libraries(Voxaudio PRIVATE Fmod::Studio Fmod::FsBank)
    target_include_directories(Voxaudio PRIVATE
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/FmodStudio>"
            "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>"
    )
elseif(USE_FMOD_CORE_BACKEND)
    target_link_libraries(Voxaudio PRIVATE Fmod::Core Fmod::FsBank)
    target_include_directories(Voxaudio PRIVATE
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/FmodCore>"
            "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>"
    )
endif()

target_link_libraries(Voxaudio PRIVATE glm yaml-cpp::yaml-cpp)
