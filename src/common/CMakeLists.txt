add_library(common STATIC)
add_library(common::common ALIAS common)
#target_compile_options(common PRIVATE -std=c++20)

target_include_directories(common PUBLIC include)

target_sources(common PRIVATE shader.cpp config.cpp debug_save_frame.cpp)

target_link_libraries(common glbinding::glbinding glbinding::glbinding-aux glm::glm spdlog::spdlog nlohmann_json::nlohmann_json lodepng::lodepng)

if (DEFINED USE_GLSL_SHADERS)
    target_compile_definitions(common PRIVATE USE_GLSL_SHADERS)
endif (DEFINED USE_GLSL_SHADERS)

if (DEFINED USE_SPIRV_SHADERS)
    target_compile_definitions(common PRIVATE USE_SPIRV_SHADERS)
endif (DEFINED USE_SPIRV_SHADERS)

if (DEFINED USE_GLFW_CONTEXT)
    add_library(context_glfw STATIC)
    add_library(common::context_glfw ALIAS context_glfw)

    target_sources(context_glfw PRIVATE context_glfw.cpp)

    target_link_libraries(context_glfw common::common glfw)
endif (DEFINED USE_GLFW_CONTEXT)

if (DEFINED USE_EGL_CONTEXT)
    add_library(context_egl STATIC)
    add_library(common::context_egl ALIAS context_egl)

    target_sources(context_egl PRIVATE context_egl.cpp)

    target_link_libraries(context_egl common::common egl::egl EGL)
endif (DEFINED USE_EGL_CONTEXT)
