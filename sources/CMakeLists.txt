cmake_minimum_required (VERSION 3.8)

project ("FnImg")
#####some global path#####
if(CMAKE_SIZEOF_VOID_P EQUAL 8) 
	set(Build_Suffix 64 CACHE INTERNAL docstring)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
	set(Build_Suffix 32 CACHE INTERNAL docstring)
endif()

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
	set(GLOB_INSTALL_PATH ${CMAKE_CURRENT_LIST_DIR}/../build/Debug_${Build_Suffix} CACHE INTERNAL docstring)
elseif(${CMAKE_BUILD_TYPE} MATCHES Release)
	set(GLOB_INSTALL_PATH ${CMAKE_CURRENT_LIST_DIR}/../build/Release_${Build_Suffix} CACHE INTERNAL docstring)
elseif(${CMAKE_BUILD_TYPE} MATCHES RelWithDebInfo)
	set(GLOB_INSTALL_PATH ${CMAKE_CURRENT_LIST_DIR}/../build/RelWithDebInfo_${Build_Suffix} CACHE INTERNAL docstring)
else()
	set(GLOB_INSTALL_PATH ${CMAKE_CURRENT_LIST_DIR}/../build/UnDefined CACHE INTERNAL docstring)
endif()
##########


file(MAKE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../build)
set(GLOBAL_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/../build/include CACHE INTERNAL docstring)
file(MAKE_DIRECTORY ${GLOBAL_INCLUDE})
#####FnCore#####
set(FNCORE_INCLUDE ${GLOBAL_INCLUDE}/FnCore CACHE INTERNAL docstring)
file(MAKE_DIRECTORY ${FNCORE_INCLUDE})
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/FnCore ${CMAKE_CURRENT_LIST_DIR}/FnCore)
##########

#####FnImgProc#####
set(FNIMGPROC_INCLUDE ${GLOBAL_INCLUDE}/FnImgProc CACHE INTERNAL docstring)
file(MAKE_DIRECTORY ${FNIMGPROC_INCLUDE})
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/FnImgProc ${CMAKE_CURRENT_LIST_DIR}/FnImgProc)
##########