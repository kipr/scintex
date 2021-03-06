project(scintex)

cmake_minimum_required(VERSION 2.8.12)
find_package(Qt5Widgets REQUIRED)

set(INCLUDE ${CMAKE_SOURCE_DIR}/include)
set(SRC ${CMAKE_SOURCE_DIR}/src)
set(DEMO_SRC ${CMAKE_SOURCE_DIR}/demo)

include_directories(${INCLUDE})

file(GLOB INCLUDES ${INCLUDE}/scintex/*.hpp)
file(GLOB SOURCES ${SRC}/*.cpp)
file(GLOB_RECURSE DEMO_SOURCES ${DEMO_SRC}/*.cpp)

set(CMAKE_AUTOMOC ON)

add_definitions(-Wall -g)

if(APPLE)
  include_directories(/usr/local/include)
  link_directories(/usr/local/lib)
  link_directories(/Library/Frameworks/)
elseif(WIN32)
	include_directories(${CMAKE_SOURCE_DIR}/../llvm/tools/clang/include)
	link_directories(${CMAKE_SOURCE_DIR}/../llvm/build/lib/Release)
endif()

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
add_library(scintex SHARED ${SOURCES} ${INCLUDES})
target_link_libraries(scintex clang)
qt5_use_modules(scintex Widgets)

link_directories(${scintex_SOURCE_DIR}/lib)

if(APPLE)
  set(OSX_INSTALL_NAMES_SCRIPT "${CMAKE_SOURCE_DIR}/scripts/osx_install_names.sh")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -headerpad_max_install_names")
  add_custom_target(osx_install_names ALL 
    COMMAND sh ${OSX_INSTALL_NAMES_SCRIPT} ${LIBRARY_OUTPUT_PATH}/libscintex.dylib
    QtCore.framework/Versions/5/QtCore
    QtGui.framework/Versions/5/QtGui
    QtWidgets.framework/Versions/5/QtWidgets
  WORKING_DIRECTORY ${LIBRARY_OUTPUT_PATH}
  DEPENDS scintex)
endif()

if(WIN32)
  install(FILES ${INCLUDES} DESTINATION ${CMAKE_SOURCE_DIR}/../prefix/include/scintex)
  install(TARGETS scintex
    ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/../prefix/lib
  RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/../prefix/lib)
else()
  install(FILES ${INCLUDES} DESTINATION include/scintex)
  install(TARGETS scintex LIBRARY DESTINATION lib)
endif()

add_executable(scintex_demo ${DEMO_SOURCES})
qt5_use_modules(scintex_demo Widgets)
target_link_libraries(scintex_demo scintex)

enable_testing()
add_subdirectory(test)
