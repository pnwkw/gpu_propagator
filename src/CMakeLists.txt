add_executable(${PROJECT_NAME})

add_subdirectory(common)
add_subdirectory(mag_field)
add_subdirectory(propagator)

target_sources(${PROJECT_NAME} PRIVATE main.cpp render_task.cpp)

target_link_libraries(${PROJECT_NAME} common::common mag_field::mag_field propagator::propagator shader::helix)

if (DEFINED USE_GLFW_CONTEXT)
    target_link_libraries(${PROJECT_NAME} common::context_glfw)
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_GLFW_CONTEXT)
endif (DEFINED USE_GLFW_CONTEXT)

if (DEFINED USE_EGL_CONTEXT)
    target_link_libraries(${PROJECT_NAME} common::context_egl)
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_EGL_CONTEXT)
endif (DEFINED USE_EGL_CONTEXT)
