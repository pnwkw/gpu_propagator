#include <glbinding-aux/ContextInfo.h>
#include <glbinding-aux/Meta.h>

#ifdef USE_GLFW_CONTEXT
#include <context_glfw.h>
#endif

#ifdef USE_EGL_CONTEXT
#include <context_egl.h>
#endif

#include <logger.h>
#include <config.h>

#include "render_task.h"

void GL_APIENTRY GLerror_callback(gl::GLenum source, gl::GLenum type, gl::GLuint id, gl::GLenum severity, gl::GLsizei length, const gl::GLchar *message, const void *userParam) {
    if (severity != gl::GL_DEBUG_SEVERITY_NOTIFICATION) {
        switch (type) {
            case gl::GLenum::GL_DEBUG_TYPE_ERROR:
                common::graphics_logger()->error("[{}] {}", glbinding::aux::Meta::getString(severity), message);
                break;
            default:
                common::graphics_logger()->info("[{}] {}", glbinding::aux::Meta::getString(severity), message);
                break;
        }
    }
}

int main() {
    auto logger_graphics = spdlog::stdout_color_mt("graphics");
    auto logger_field = spdlog::stdout_color_mt("field");

    if constexpr (common::isDebug) {
        common::graphics_logger()->set_level(spdlog::level::debug);
        common::field_logger()->set_level(spdlog::level::debug);
        spdlog::set_level(spdlog::level::debug);
    }

    common::configRead("config.json");

#ifdef USE_GLFW_CONTEXT
    common::context_glfw context;
#endif

#ifdef USE_EGL_CONTEXT
    common::context_egl context;
#endif

    context.create();

    glbinding::initialize(context.getProcAddressPtr(), true);

    if constexpr (common::isDebug) {
        if (glbinding::aux::ContextInfo::supported({gl::GLextension::GL_KHR_debug})) {
            gl::ContextFlagMask ctxFlag;
            gl::glGetIntegerv(gl::GL_CONTEXT_FLAGS, reinterpret_cast<gl::GLint *>(&ctxFlag));
            if ((ctxFlag & gl::GL_CONTEXT_FLAG_DEBUG_BIT) == gl::GL_CONTEXT_FLAG_DEBUG_BIT) {
                gl::glDebugMessageCallback(GLerror_callback, nullptr);
                gl::glEnable(gl::GL_DEBUG_OUTPUT);
                gl::glEnable(gl::GL_DEBUG_OUTPUT_SYNCHRONOUS);
            }
        } else {
            common::graphics_logger()->warn("{} not supported. OpenGL logging disabled.", glbinding::aux::Meta::getString(gl::GLextension::GL_KHR_debug));
        }
    }

    {
        RenderTask task(context);

        task.loadParticles(common::samples);

        if (common::isBenchmark) {
            task.doBenchmark();
        } else {
            task.drawLoop();
        }
    }

    gl::glFinish();

    return 0;
}