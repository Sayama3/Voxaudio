cmake_minimum_required(VERSION 3.23)

# Thanks to https://github.com/ezEngine/thirdparty-fmod/blob/af3ed3432bb5480cc63ce7f49db5dd68bb073d2a/CMakeLists.txt for finding out how to do it.

#option(FMOD_USE_CUSTOM_INSTALL_PATH "If on, the library will search for the 'FMOD_INSTALL_PATH' instead of the one already installed." OFF)
option(FMOD_INSTALL_PATH, "The custom install path of fmod")

if(DEFINED ENV{FMOD_INSTALL_PATH})
	set(FMOD_INSTALL_PATH "$ENV{FMOD_INSTALL_PATH}")
	message(STATUS "Found FMOD_INSTALL_PATH in environment variables : $ENV{FMOD_INSTALL_PATH}")
else()
	message(STATUS "Environment Variable not found.")
endif()

# TODO: Add all the version supported by fmod
# TODO: Only use the installed version of Fmod. So no legal problems.

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	if(DEFINED FMOD_INSTALL_PATH)
		set(FMOD_SEARCH_PATH ${FMOD_INSTALL_PATH})
	else()
		message(FATAL_ERROR "No default install path for linux.")
	endif()
	set(FMOD_DLL_SUFFIX ".so.12")
	set(FMOD_LIB_SUFFIX ".a")
	set(FMOD_PREFIX "lib")
elseif (CMAKE_SYSTEM_NAME STREQUAL "WindowsStore")
	# UWP builds
	
	if(DEFINED ${FMOD_INSTALL_PATH})
		set(FMOD_SEARCH_PATH ${FMOD_INSTALL_PATH})
	else()
		set(FMOD_SEARCH_PATH "[HKEY_CURRENT_USER\\Software\\FMOD\ Studio\ API\ Universal\ Windows\ Platform]")
	endif()
		
	set(FMOD_DLL_SUFFIX ".dll")
	set(FMOD_LIB_SUFFIX ".lib")
	set(FMOD_PREFIX "")
		
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	# Desktop Windows builds
	if(DEFINED FMOD_INSTALL_PATH)
		set(FMOD_SEARCH_PATH ${FMOD_INSTALL_PATH})
	else()
		set(FMOD_SEARCH_PATH "[HKEY_CURRENT_USER\\Software\\FMOD\ Studio\ API\ Windows]")
	endif()

	set(FMOD_DLL_SUFFIX ".dll")
	set(FMOD_LIB_SUFFIX "_vc.lib")
	set(FMOD_PREFIX "")
else()
	message(FATAL_ERROR "The platform ${CMAKE_SYSTEM_NAME} is not supported yet for CMAKE fmod.")
endif()

find_path(FMOD_SDK_LOCATION api/core/inc/fmod.h PATHS ${FMOD_SEARCH_PATH})

message(STATUS "FMOD_SDK_LOCATION is ${FMOD_SDK_LOCATION}")

set (FMOD_DIR_STUDIO "${FMOD_SDK_LOCATION}/api/studio")
set (FMOD_DIR_FSBANK "${FMOD_SDK_LOCATION}/api/fsbank")
set (FMOD_DIR_CORE "${FMOD_SDK_LOCATION}/api/core")

#TODO: find a way to check if we use an ARM architecture.
set(VXM_CMAKE_ARCHITECTURE_ARM OFF)

if (VXM_CMAKE_ARCHITECTURE_ARM)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		message(FATAL_ERROR "Fmod does not support arm64, or support needs to be added to EZ once binaries become available")
	else()
		set(FMOD_LIB_ARCH "arm")
	endif()
else()
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(FMOD_LIB_ARCH "x64")
	else()
		set(FMOD_LIB_ARCH "x86")
	endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Fmod DEFAULT_MSG FMOD_SDK_LOCATION)

if (FMOD_FOUND)

	add_library(Fmod::Core SHARED IMPORTED GLOBAL)
	set_target_properties(Fmod::Core PROPERTIES IMPORTED_LOCATION "${FMOD_DIR_CORE}/lib/${FMOD_LIB_ARCH}/${FMOD_PREFIX}fmod${FMOD_DLL_SUFFIX}")
	set_target_properties(Fmod::Core PROPERTIES IMPORTED_LOCATION_DEBUG "${FMOD_DIR_CORE}/lib/${FMOD_LIB_ARCH}/${FMOD_PREFIX}fmod${FMOD_DEBUG_SUFFIX}${FMOD_DLL_SUFFIX}")
	set_target_properties(Fmod::Core PROPERTIES IMPORTED_IMPLIB "${FMOD_DIR_CORE}/lib/${FMOD_LIB_ARCH}/${FMOD_PREFIX}fmod${FMOD_LIB_SUFFIX}")
	set_target_properties(Fmod::Core PROPERTIES IMPORTED_IMPLIB_DEBUG "${FMOD_DIR_CORE}/lib/${FMOD_LIB_ARCH}/${FMOD_PREFIX}fmod${FMOD_DEBUG_SUFFIX}${FMOD_LIB_SUFFIX}")
	set_target_properties(Fmod::Core PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${FMOD_DIR_CORE}/inc")

	add_library(Fmod::FsBank SHARED IMPORTED GLOBAL)
	set_target_properties(Fmod::FsBank PROPERTIES IMPORTED_LOCATION "${FMOD_DIR_FSBANK}/lib/${FMOD_LIB_ARCH}/${FMOD_PREFIX}fsbank${FMOD_DLL_SUFFIX}")
	set_target_properties(Fmod::FsBank PROPERTIES IMPORTED_LOCATION_DEBUG "${FMOD_DIR_FSBANK}/lib/${FMOD_LIB_ARCH}/${FMOD_PREFIX}fsbank${FMOD_DEBUG_SUFFIX}${FMOD_DLL_SUFFIX}")
	set_target_properties(Fmod::FsBank PROPERTIES IMPORTED_IMPLIB "${FMOD_DIR_FSBANK}/lib/${FMOD_LIB_ARCH}/${FMOD_PREFIX}fsbank${FMOD_LIB_SUFFIX}")
	set_target_properties(Fmod::FsBank PROPERTIES IMPORTED_IMPLIB_DEBUG "${FMOD_DIR_FSBANK}/lib/${FMOD_LIB_ARCH}/${FMOD_PREFIX}fsbank${FMOD_DEBUG_SUFFIX}${FMOD_LIB_SUFFIX}")
	set_target_properties(Fmod::FsBank PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${FMOD_DIR_FSBANK}/inc")

	add_library(Fmod::Studio SHARED IMPORTED GLOBAL)
	set_target_properties(Fmod::Studio PROPERTIES IMPORTED_LOCATION "${FMOD_DIR_STUDIO}/lib/${FMOD_LIB_ARCH}/${FMOD_PREFIX}fmodstudio${FMOD_DLL_SUFFIX}")
	set_target_properties(Fmod::Studio PROPERTIES IMPORTED_LOCATION_DEBUG "${FMOD_DIR_STUDIO}/lib/${FMOD_LIB_ARCH}/${FMOD_PREFIX}fmodstudio${FMOD_DEBUG_SUFFIX}${FMOD_DLL_SUFFIX}")
	set_target_properties(Fmod::Studio PROPERTIES IMPORTED_IMPLIB "${FMOD_DIR_STUDIO}/lib/${FMOD_LIB_ARCH}/${FMOD_PREFIX}fmodstudio${FMOD_LIB_SUFFIX}")
	set_target_properties(Fmod::Studio PROPERTIES IMPORTED_IMPLIB_DEBUG "${FMOD_DIR_STUDIO}/lib/${FMOD_LIB_ARCH}/${FMOD_PREFIX}fmodstudio${FMOD_DEBUG_SUFFIX}${FMOD_LIB_SUFFIX}")
	set_target_properties(Fmod::Studio PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${FMOD_DIR_STUDIO}/inc")
	target_link_libraries(Fmod::Studio INTERFACE Fmod::Core)

endif()

mark_as_advanced(FORCE FMOD_SDK_LOCATION)