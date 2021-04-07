# CMakeList.txt: Test_SDK 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

project ("FnCore")

add_definitions(-DUNICODE -D_UNICODE)

if(MSVC)
    add_definitions(-DCRT_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS)
endif()
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O2 -Wall") 

set(CMAKE_CXX_STANDARD 14)
if(UNIX)
add_definitions(-std=c++14)
endif()

file(GLOB core_head ${CMAKE_CURRENT_LIST_DIR}/*.h
                    ${CMAKE_CURRENT_LIST_DIR}/*.hpp)

file(GLOB core_src ${CMAKE_CURRENT_LIST_DIR}/*.cpp)


#拷贝头文件到相关目录里   开启了一个进程
#execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${core_head}  ${FNCORE_INCLUDE})

file(COPY ${core_head} DESTINATION ${FNCORE_INCLUDE})

add_library(${PROJECT_NAME}
	   SHARED
	   ${core_head}
       ${core_src})


set(CMAKE_INSTALL_DIR ${GLOB_INSTALL_PATH})

message(CMAKE_INSTALL_DIR=${CMAKE_INSTALL_DIR})

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_INSTALL_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_INSTALL_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_INSTALL_DIR}
)

install(FILES ${core_head} DESTINATION ${FNCORE_INCLUDE})

