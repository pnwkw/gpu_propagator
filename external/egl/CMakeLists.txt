add_library(egl INTERFACE)
add_library(egl::egl ALIAS egl)

target_include_directories(egl INTERFACE include)