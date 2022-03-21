#ifndef COMMON_DEBUG_SAVE_FRAME_H
#define COMMON_DEBUG_SAVE_FRAME_H

#include <string>

namespace common {
    class debug_save_frame {
    public:
        static void save(std::string fileName);
    };
}// namespace viz

#endif//COMMON_DEBUG_SAVE_FRAME_H
