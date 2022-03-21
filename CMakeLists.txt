cmake_minimum_required(VERSION 3.16)

project(quark VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

# EGL can be used to execute on systems with no graphics (text mode), otherwise GLFW
set(USE_GLFW_CONTEXT true)
#set(USE_EGL_CONTEXT true)

# SPIRV shaders do not work with cache version of shaders due to gl_VertexID incompatibility
#set(USE_SPIRV_SHADERS true)
set(USE_GLSL_SHADERS true)

# GLFW
if(DEFINED USE_GLFW_CONTEXT)
    set(GLFW_BUILD_DOCS        OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS       OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES    OFF CACHE BOOL "" FORCE)
    set(GLFW_USE_HYBRID_HPG    ON  CACHE BOOL "" FORCE)
    add_subdirectory(external/glfw)
endif(DEFINED USE_GLFW_CONTEXT)

# EGL
if(DEFINED USE_EGL_CONTEXT)
    add_subdirectory(external/egl)
endif(DEFINED USE_EGL_CONTEXT)

# GLM
add_subdirectory(external/glm)

if(DEFINED USE_GLSL_SHADERS)
    include(cmake/BuildGLSL.cmake)
endif(DEFINED USE_GLSL_SHADERS)

#if(DEFINED USE_SPIRV_SHADERS)
#    include(cmake/BuildSPIRV.cmake)
#    # We need Vulkan compiler to compile SPIRV shaders
#    find_package(Vulkan REQUIRED)
#endif(DEFINED USE_SPIRV_SHADERS)

# glbinding
set(BUILD_SHARED_LIBS      OFF CACHE BOOL "" FORCE)
set(OPTION_BUILD_EXAMPLES  OFF CACHE BOOL "" FORCE)
set(OPTION_BUILD_TOOLS     OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glbinding)
# spdlog
add_subdirectory(external/spdlog)
# json
add_subdirectory(external/json)
# lodepng
add_subdirectory(external/lodepng)

# code
add_subdirectory(src)
# shaders
add_subdirectory(shaders)

# data files
file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY config.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
