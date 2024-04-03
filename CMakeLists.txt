#checkout https://github.com/ttroy50/cmake-examples

cmake_minimum_required(VERSION 3.24)

project(OpenGL-Playground C CXX) #C needed for GLAD

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/lib/)
set(CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/lib/)

find_package(OpenGL REQUIRED)

### enable intrinsics ###
if(MSVC)
    add_compile_options(/arch:SSE2 /arch:AVX2)
else()
    add_compile_options(-msse2 -mavx2)
endif()

### GLFW ###
#disable unneccesary GLFW CMake stuff
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/glfw)


### GLAD ###
set(GLAD_SOURCES_DIR ${CMAKE_SOURCE_DIR}/lib/glad/)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/glad/cmake glad_cmake)
glad_add_library(glad_gl_core_33 STATIC API gl:core=3.3)

### GLM ###
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)
find_package(glm CONFIG REQUIRED)

### imgui source ###
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/lib/imgui/)
FILE(GLOB IMGUI_SOURCES ${IMGUI_DIR}/*.cpp IMGUI_SOURCES ${IMGUI_DIR}/*.h ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp)

set(LIB_INCLUDES
PUBLIC
${IMGUI_DIR}
${CMAKE_SOURCE_DIR}/lib/stb/
${GLM_INCLUDE_DIR}
)

include_directories(${LIB_INCLUDES})
add_library(imgui STATIC ${IMGUI_SOURCES})
target_link_libraries(imgui PRIVATE glfw)
### Target main ####

set(SOURCES
${CMAKE_SOURCE_DIR}/src/main.cpp
${CMAKE_SOURCE_DIR}/src/procedural_meshes.cpp
)

add_executable(main ${SOURCES})

target_include_directories(main PRIVATE ${LIB_INCLUDES} ${CMAKE_SOURCE_DIR}/include)

set(LIBS
PRIVATE
glfw
imgui
glad_gl_core_33
OpenGL::GL
glm::glm
)

target_link_libraries(main PUBLIC ${LIBS})

### copy resources alognside executable

add_custom_command(
  TARGET main POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources/
    $<TARGET_FILE_DIR:main>)


######




### cubes ###

add_executable(cubes ${CMAKE_SOURCE_DIR}/src/cubes/main.cpp)
target_include_directories(cubes PRIVATE ${CMAKE_SOURCE_DIR}/src/cubes/)
target_link_libraries(cubes PUBLIC ${LIBS})




