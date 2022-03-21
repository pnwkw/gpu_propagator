add_library(propagator STATIC)
add_library(propagator::propagator ALIAS propagator)

target_include_directories(propagator PUBLIC include)

target_sources(propagator PRIVATE helix.cpp propagator.cpp)

target_link_libraries(propagator glm::glm)
