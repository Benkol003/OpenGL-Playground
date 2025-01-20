#checkout https://github.com/ttroy50/cmake-examples

cmake_minimum_required(VERSION 3.24)

project(OpenGL-Playground C CXX) #C needed for GLAD

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/lib/)
set(CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/lib/)


########## libs not used in emscripten ########
if(NOT EMSCRIPTEN)
find_package(OpenGL REQUIRED)

if(MSVC)
    add_compile_options(/arch:SSE2 /arch:AVX2)
elseif(UNIX)
    add_compile_options(-msse2 -mavx2)
endif()

### GLFW ###
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/glfw)


### GLAD ###
set(GLAD_SOURCES_DIR ${CMAKE_SOURCE_DIR}/lib/glad/)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/glad/cmake glad_cmake)
glad_add_library(glad_gl_core_33 STATIC API gl:core=3.3)
endif()

######### libs globally used ############

### fmt ###
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/fmt/)

### GLM ###
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/glm)

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
if(NOT EMSCRIPTEN)
target_link_libraries(imgui PRIVATE glfw)
endif()
### Target main ####

file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/include.*.h ${CMAKE_SOURCE_DIR}/include/*.hpp)
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp) #glob recursive picks up cubes aswell

list(APPEND SOURCES ${HEADERS}) #to show "Header Files" folder in VS

add_executable(main ${SOURCES})

target_include_directories(main PRIVATE ${LIB_INCLUDES} ${CMAKE_SOURCE_DIR}/include)

if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set_target_properties(main PROPERTIES 
        COMPILE_FLAGS " -Wno-everything -O3 -s FETCH -s WASM=1 -s USE_WEBGL2=1 -s USE_GLFW=3 -s FULL_ES3=1 -s ASSERTIONS=1 -s DISABLE_EXCEPTION_CATCHING=0 -sALLOW_MEMORY_GROWTH" 
        LINK_FLAGS "-O3 -s FETCH -s WASM=1 -s USE_WEBGL2=1 -s USE_GLFW=3 -s FULL_ES3=1 -s ASSERTIONS=1 -s DISABLE_EXCEPTION_CATCHING=0 -sALLOW_MEMORY_GROWTH"
    )

    set(LIBS
    PRIVATE
    glfw
    imgui
    glm::glm
    fmt
    )
    
else()
    set(LIBS
    PRIVATE
    glfw
    imgui
    glad_gl_core_33
    OpenGL::GL
    glm::glm
    fmt
    )
endif()

target_link_libraries(main PUBLIC ${LIBS})

### copy shader alongside executable

if(EMSCRIPTEN)
add_custom_command(
  TARGET main POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources/ES/
    $<TARGET_PROPERTY:main,BINARY_DIR>/)
else()
add_custom_command(
  TARGET main POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources/
    $<TARGET_PROPERTY:main,BINARY_DIR>/)
endif()

######