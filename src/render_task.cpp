#include "render_task.h"

#include <algorithm>
#include <fstream>
#include <span>

#include <glm/gtc/matrix_transform.hpp>
#include <nlohmann/json.hpp>

#include <debug_save_frame.h>
#include <logger.h>
#include <propagator.h>
#include <shader.h>

RenderTask::RenderTask(common::context_base &context) : m_context(context) {
    m_glsl = std::make_unique<viz::glsl>();

    gl::glCreateVertexArrays(1, &m_vao);
    gl::glCreateBuffers(1, &m_vertex_buffer);

    gl::glEnableVertexArrayAttrib(m_vao, Point::POS_ATTRIB);
    gl::glVertexArrayAttribFormat(m_vao, Point::POS_ATTRIB, decltype(Point::pos)::length(), gl::GL_FLOAT, gl::GL_FALSE, offsetof(Point, pos));

    gl::glVertexArrayVertexBuffer(m_vao, VTX_BINDING, m_vertex_buffer, 0, sizeof(Point));
    gl::glVertexArrayAttribBinding(m_vao, Point::POS_ATTRIB, VTX_BINDING);

    gl::glEnableVertexArrayAttrib(m_vao, Point::MOMENTUM_ATTRIB);
    gl::glVertexArrayAttribFormat(m_vao, Point::MOMENTUM_ATTRIB, decltype(Point::momentum)::length(), gl::GL_FLOAT, gl::GL_FALSE, offsetof(Point, momentum));

    gl::glEnableVertexArrayAttrib(m_vao, Point::CHARGE_ATTRIB);
    gl::glVertexArrayAttribFormat(m_vao, Point::CHARGE_ATTRIB, 1, gl::GL_FLOAT, gl::GL_FALSE, offsetof(Point, charge));

    gl::glVertexArrayAttribBinding(m_vao, Point::MOMENTUM_ATTRIB, VTX_BINDING);
    gl::glVertexArrayAttribBinding(m_vao, Point::CHARGE_ATTRIB, VTX_BINDING);

    gl::glCreateBuffers(1, &m_ubo);
    gl::glNamedBufferData(m_ubo, sizeof(State), &m_state, gl::GL_STREAM_DRAW);

    gl::GLuint vertexShader, geometryShader, fragShader;

    common::graphics_logger()->info("Compiling shaders...");

    vertexShader = common::loadCompileShader(gl::GL_VERTEX_SHADER, "helix_v");

    std::string geomShaderName = fmt::format("helix_g_{}", common::method);

    geometryShader = common::loadCompileShader(gl::GL_GEOMETRY_SHADER, geomShaderName);
    fragShader = common::loadCompileShader(gl::GL_FRAGMENT_SHADER, common::method == "glsl" ? "helix_blue_f" : "helix_red_f");

    m_program = common::createProgram({vertexShader, geometryShader, fragShader}, common::isBenchmark ? "outVert" : nullptr);

    common::graphics_logger()->info("Done compiling shaders!");

    gl::glDeleteShader(vertexShader);
    gl::glDeleteShader(geometryShader);
    gl::glDeleteShader(fragShader);

    gl::glCreateBuffers(1, &m_xfb_buffer);
    const auto xfbBytes = common::pointsPerTrack * common::samples * sizeof(glm::vec3);
    gl::glNamedBufferData(m_xfb_buffer, xfbBytes, nullptr, gl::GL_STATIC_READ);
}

RenderTask::~RenderTask() {
    gl::glDeleteProgram(m_program);
    gl::glDeleteBuffers(1, &m_xfb_buffer);
    gl::glDeleteBuffers(1, &m_vertex_buffer);
    gl::glDeleteBuffers(1, &m_ubo);
    gl::glDeleteVertexArrays(1, &m_vao);
}

void RenderTask::loadParticles(std::size_t count) {
    m_vertices.clear();

    std::ifstream i("data/events.json");

    nlohmann::json events;
    i >> events;

    std::size_t particles = 0;

    for(const auto &event: events) {
        for(const auto &track : event["tracks"]) {
            m_vertices.emplace_back(
                    glm::vec4(track["X"], track["Y"], track["Z"], 1),
                    glm::vec4(track["px"], track["py"], track["pz"], 0),
                    track["charge"]
            );

            particles++;

            if (m_vertices.size() == common::samples) {
                goto vectorLoaded;
            }
        }
    }

vectorLoaded:

    gl::glNamedBufferData(m_vertex_buffer, std::span(m_vertices).size_bytes(), m_vertices.data(), gl::GL_STATIC_DRAW);

    common::graphics_logger()->info("Loaded {} particles.", particles);
}

void RenderTask::drawLoop() {
    std::size_t iFrame = 0;

    gl::glPointSize(5.0f);

    const auto inputSize = m_vertices.size();

    while(!m_context.shouldClose()) {
        m_context.pollEvents();

        gl::glViewport(0, 0, common::windowWidth, common::windowHeight);
        gl::glClear(gl::GL_COLOR_BUFFER_BIT);

        const auto frac = 100.0f;
        const auto angle = glm::pi<float>() / 2;
        const auto cameraPos = glm::vec3{glm::cos(angle) * 2000.0f, 0.0f, glm::sin(angle) * 2000.0f};

        glm::mat4 viewMatrix = glm::lookAt(cameraPos, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f});
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f),
                                                      common::windowWidth / static_cast<float>(common::windowHeight),
                                                      1.0f, 6000.0f);
        glm::mat4 modelMatrix = glm::mat4(1.0f);

        m_state.MVP = projectionMatrix * viewMatrix * modelMatrix;

        gl::glNamedBufferSubData(m_ubo, 0, sizeof(State), &m_state);

        gl::glBindVertexArray(m_vao);
        gl::glUseProgram(m_program);
        gl::glBindBufferBase(gl::GL_UNIFORM_BUFFER, 0, m_ubo);

        if (common::method == "glsl") {
            gl::glBindBufferBase(gl::GL_SHADER_STORAGE_BUFFER, 1, m_glsl->getSolSegmentsBufferName());
            gl::glBindBufferBase(gl::GL_SHADER_STORAGE_BUFFER, 2, m_glsl->getDipSegmentsBufferName());
            gl::glBindBufferBase(gl::GL_SHADER_STORAGE_BUFFER, 3, m_glsl->getSolParamsBufferName());
            gl::glBindBufferBase(gl::GL_SHADER_STORAGE_BUFFER, 4, m_glsl->getDipParamsBufferName());
        }

        const auto start = std::chrono::high_resolution_clock::now();

        gl::glDrawArrays(gl::GL_POINTS, 0, inputSize);

        const auto end = std::chrono::high_resolution_clock::now();

        m_context.swapBuffers();

        common::debug_save_frame::save("sample.png");

        ++iFrame;
    }
}

std::tuple<double, double, double, double, double> RenderTask::benchmark() {
    const auto inputSize = m_vertices.size();

    std::vector<glm::vec3> points_result(common::pointsPerTrack * inputSize);
    std::fill(points_result.begin(), points_result.end(), glm::vec3(0, 0, 0));

    gl::glEnable(gl::GL_RASTERIZER_DISCARD);

    gl::glBindVertexArray(m_vao);
    gl::glUseProgram(m_program);
    gl::glBindBufferBase(gl::GL_UNIFORM_BUFFER, 0, m_ubo);
    gl::glBindBufferBase(gl::GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_xfb_buffer);

    if (common::method == "glsl") {
        gl::glBindBufferBase(gl::GL_SHADER_STORAGE_BUFFER, 1, m_glsl->getSolSegmentsBufferName());
        gl::glBindBufferBase(gl::GL_SHADER_STORAGE_BUFFER, 2, m_glsl->getDipSegmentsBufferName());
        gl::glBindBufferBase(gl::GL_SHADER_STORAGE_BUFFER, 3, m_glsl->getSolParamsBufferName());
        gl::glBindBufferBase(gl::GL_SHADER_STORAGE_BUFFER, 4, m_glsl->getDipParamsBufferName());
    }

    gl::glBeginTransformFeedback(gl::GL_POINTS);

    const auto start = std::chrono::high_resolution_clock::now();

    gl::glDrawArrays(gl::GL_POINTS, 0, inputSize);

    gl::glEndTransformFeedback();

    gl::glGetNamedBufferSubData(m_xfb_buffer, 0, std::span(points_result).size_bytes(), points_result.data());

    const auto end = std::chrono::high_resolution_clock::now();

    gl::glDisable(gl::GL_RASTERIZER_DISCARD);

    std::size_t index = 0;

    double RMSE = 0.0, RMSEx = 0.0, RMSEy = 0.0, RMSEz = 0.0;

    for(int i = 0; i < inputSize; i++) {
        const auto &point = m_vertices[i];

        Propagator prop(point.pos, point.momentum, point.charge, common::pointsPerTrack);
        prop.MakeTrack(true);

        for (std::size_t j = 0; j < common::pointsPerTrack; j++) {
            const glm::dvec3 v1 = points_result[index];
            glm::dvec3 v2;

            if (j < prop.points.size()) {
                v2 = prop.points[j];
            } else {
                v2 = glm::dvec3(0);
            }

            const auto diff = v1 - v2;

            RMSEx += diff.x * diff.x;
            RMSEy += diff.y * diff.y;
            RMSEz += diff.z * diff.z;

            const auto distance = glm::distance(v1, v2);
            RMSE += (distance * distance) / (decltype(v1)::length());

            index++;
        }
    }

    RMSE = glm::sqrt(RMSE / index);
    RMSEx = glm::sqrt(RMSEx / index);
    RMSEy = glm::sqrt(RMSEy / index);
    RMSEz = glm::sqrt(RMSEz / index);

    const auto totalTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1e6;

    return std::make_tuple(totalTime, RMSE, RMSEx, RMSEy, RMSEz);
}

void RenderTask::doBenchmark() {
    std::ofstream outfile(fmt::format("{}/{}_{}.csv", common::resultsDir, common::method, common::samples));

    if (outfile.good()) {
        outfile << "Time;RMSE;RMSEx;RMSEy;RMSEz" << std::endl;

        double total_duration = 0, total_RMSEt = 0, total_RMSEx = 0, total_RMSEy = 0, total_RMSEz = 0;

        for (std::size_t i = 0; i < common::runs; i++) {
            auto const [duration, RMSEt, RMSEx, RMSEy, RMSEz] = benchmark();

            total_duration += duration;
            total_RMSEt += RMSEt;
            total_RMSEx += RMSEx;
            total_RMSEy += RMSEy;
            total_RMSEz += RMSEz;
        }

        total_duration /= common::runs;
        total_RMSEt /= common::runs;
        total_RMSEx /= common::runs;
        total_RMSEy /= common::runs;
        total_RMSEz /= common::runs;

        outfile << total_duration << ";" << total_RMSEt << ";" << total_RMSEx << ";" << total_RMSEy << ";" << total_RMSEz << std::endl;
    }

    outfile.close();
}