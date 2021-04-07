# CMakeList.txt: FnImgProc 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 14)
project ("FnImgProc")

if(UNIX)
add_definitions(-std=c++14)
endif()

# 将源代码添加到此项目的可执行文件。

file(GLOB FnImgPro_Head ${CMAKE_CURRENT_SOURCE_DIR}/*.h
						${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

file(GLOB FnImgPro_Src ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)


#execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${FnImgPro_Head} ${FNIMGPROC_INCLUDE})
file(COPY ${FnImgPro_Head} DESTINATION ${FNIMGPROC_INCLUDE})

add_library (${PROJECT_NAME}
	         SHARED
	         ${FnImgPro_Head}
			 ${FnImgPro_Src})

message(GLOBAL_INCLUDE=${GLOBAL_INCLUDE})
target_include_directories(
	${PROJECT_NAME}
		PRIVATE
	${GLOBAL_INCLUDE}
)

set(CMAKE_INSTALL_DIR ${GLOB_INSTALL_PATH})

message(GLOB_INSTALL_PATH=${GLOB_INSTALL_PATH})

target_link_libraries(${PROJECT_NAME}
	PRIVATE
	FnCore
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_INSTALL_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_INSTALL_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_INSTALL_DIR}
)


#set (CMAKE_INSTALL_BINDIR ${CMAKE_INSTALL_PREFIX}/bin)
#set (CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_PREFIX}/lib)

install(FILES ${FnImgPro_Head} DESTINATION ${CMAKE_CURRENT_LIST_DIR})

#install(TARGETS IOSensorAcquisition
 #       EXPORT IOSensorAcquisition
  #      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
   #     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    #    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
#)