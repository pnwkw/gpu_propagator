#ifndef QUARK_RENDERTASK_H
#define QUARK_RENDERTASK_H

#include <vector>

#include <glm/glm.hpp>
#include <glbinding/gl46core/gl.h>

#include <context_base.h>
#include <glsl.h>

class RenderTask {
private:
    struct State {
        glm::mat4 MVP;
    };

    struct Point {
        glm::vec4 pos;
        glm::vec4 momentum;
        float charge;
        constexpr static gl::GLuint POS_ATTRIB = 0;
        constexpr static gl::GLuint MOMENTUM_ATTRIB = 1;
        constexpr static gl::GLuint CHARGE_ATTRIB = 2;
    };

    constexpr static gl::GLuint VTX_BINDING = 0;

    common::context_base &m_context;

    std::vector<Point> m_vertices;

    std::unique_ptr<viz::glsl> m_glsl;

    State m_state{};

    gl::GLuint m_vao{};
    gl::GLuint m_ubo{};
    gl::GLuint m_vertex_buffer{};
    gl::GLuint m_xfb_buffer{};
    gl::GLuint m_program{};

    std::tuple<double, double, double, double, double> benchmark();

public:
    RenderTask(common::context_base &context);
    ~RenderTask();
    void loadParticles(std::size_t count);
    void drawLoop();
    void doBenchmark();
};


#endif //QUARK_RENDERTASK_H
