add_library(lodepng STATIC lodepng.cpp)
add_library(lodepng::lodepng ALIAS lodepng)

target_include_directories(lodepng PUBLIC include)
