# CMakeList.txt : CMake project for CG, include source and define project specific logic here.
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast")

project("CG")

##### source files #####
file(GLOB PROJECT_SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/*.c")

#imgui#
file(GLOB IMGUI_SOURCES external/imgui/*.cpp)
list(APPEND IMGUI_SOURCES "external/imgui/backends/imgui_impl_glfw.cpp")
list(APPEND IMGUI_SOURCES "external/imgui/backends/imgui_impl_opengl3.cpp")
list(APPEND IMGUI_SOURCES "external/imgui/misc/cpp/imgui_stdlib.cpp")
list(APPEND IMGUI_SOURCES "external/imgui/misc/debuggers/imgui.natvis" "external/imgui/misc/debuggers/imgui.natstepfilter")
#### executable ####
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${IMGUI_SOURCES} "${PROJECT_SOURCE_DIR}/external/glad.c" "external/eigen.natvis")

# Configure a header file to pass the assets directory to the source code
set(RESOURCES_DIR "${CMAKE_SOURCE_DIR}/resources")
configure_file("${PROJECT_SOURCE_DIR}/config/PathConfig.h.in" "${CMAKE_BINARY_DIR}/config/PathConfig.h")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/config")

##### link libraries #####
#GLFW#
if(MSVC)
  set(GLFW_LIB_PATH "${PROJECT_SOURCE_DIR}/libs/lib-vc2022")
elseif(UNIX AND NOT APPLE) # Check if the system is Linux
  set(GLFW_LIB_PATH "${PROJECT_SOURCE_DIR}/libs")
else()
  # Set the path for GLFW for non-MSVC compilers here
  set(GLFW_LIB_PATH "[path to your non-MSVC glfw lib directory]")
endif()

find_library(GLFW_LIBRARY NAMES glfw3 glfw PATHS ${GLFW_LIB_PATH} NO_DEFAULT_PATH)
if(NOT GLFW_LIBRARY)
  message(FATAL_ERROR "GLFW library not found in ${GLFW_LIB_PATH}")
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC ${GLFW_LIBRARY})
if(WIN32)
  target_compile_definitions(${PROJECT_NAME} PUBLIC GLFW_INCLUDE_NONE)
endif()

##### include directories #####
target_include_directories(${PROJECT_NAME} PUBLIC "include")
#imgui#
target_include_directories(${PROJECT_NAME} PUBLIC "external/imgui")
target_include_directories(${PROJECT_NAME} PUBLIC "external/imgui/backends")
target_include_directories(${PROJECT_NAME} PUBLIC "external/imgui/misc/cpp")


if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-narrowing")
endif()