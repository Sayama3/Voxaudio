cmake_minimum_required(VERSION 3.23)

project(Voxaudio VERSION 0.0.1 DESCRIPTION "A multi backend audio engine.")

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_COMPUTED_DEFAULT "20")

option(USE_FMOD_STUDIO_BACKEND "Use Fmod Studio for the backend." OFF)
option(USE_FMOD_CORE_BACKEND "Use Fmod Core for the backend." ON)

add_subdirectory(lib/fmod)

set(FMOD_STUDIO_SRC_FILES 
    "src/FmodStudio/Voxaudio.cpp"
    "src/FmodStudio/FmodStudioEngine.hpp"
    "src/FmodStudio/FmodStudioEngine.cpp"
    "src/FmodStudio/FmodStudioSound.hpp"
    "src/FmodStudio/FmodStudioSound.cpp"
)

set(FMOD_CORE_SRC_FILES 
    "src/FmodCore/Voxaudio.cpp"
    "src/FmodCore/FmodCoreEngine.hpp"
    "src/FmodCore/FmodCoreEngine.cpp"
)

if(USE_FMOD_STUDIO_BACKEND)
    set(SRC_FILES ${FMOD_STUDIO_SRC_FILES})
elseif(USE_FMOD_CORE_BACKEND)
    set(SRC_FILES ${FMOD_CORE_SRC_FILES})
else()
    message(FATAL_ERROR "No backend selected...")
endif()

add_library(Voxaudio ${SRC_FILES})


set_target_properties(Voxaudio PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(Voxaudio PROPERTIES SOVERSION 0)

target_include_directories(Voxaudio PUBLIC include)


set_target_properties(Voxaudio PROPERTIES
        VERSION       ${${PROJECT_NAME}_VERSION}
        PUBLIC_HEADER "include/Voxaudio.hpp"
        PUBLIC cxx_std_20
)

target_include_directories(Voxaudio PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>"
)

if(USE_FMOD_STUDIO_BACKEND)
    target_link_libraries(Voxaudio PRIVATE Fmod::Studio Fmod::FsBank)
    target_include_directories(Voxaudio PRIVATE
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/FmodStudio>"
    )
elseif(USE_FMOD_CORE_BACKEND)
    target_link_libraries(Voxaudio PRIVATE Fmod::Core Fmod::FsBank)
    target_include_directories(Voxaudio PRIVATE
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/FmodCore>"
    )
endif()
