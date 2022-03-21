#include "debug_save_frame.h"

#include <glbinding/gl46core/gl.h>
#include <config.h>
#include <lodepng.h>
#include <algorithm>

void common::debug_save_frame::save(std::string fileName) {
    std::vector<std::uint8_t> pixels(common::windowWidth * common::windowHeight * 4);

    gl::glPixelStorei(gl::GL_PACK_ALIGNMENT, 1);
    gl::glReadPixels(0, 0, common::windowWidth, common::windowHeight, gl::GL_RGBA, gl::GL_UNSIGNED_BYTE, pixels.data());

    for(int line = 0; line != common::windowHeight/2; ++line) {
        std::swap_ranges(pixels.begin() + 4 * common::windowWidth * line,
                         pixels.begin() + 4 * common::windowWidth * (line+1),
                         pixels.begin() + 4 * common::windowWidth * (common::windowHeight-line-1));
    }

    lodepng::encode(fileName.c_str(), pixels.data(), common::windowWidth, common::windowHeight);
}
