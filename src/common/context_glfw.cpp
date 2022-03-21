#include "context_glfw.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "config.h"

void error_callback(int error, const char *description) {
	spdlog::get("glfw")->error("Error {}: {}", error, description);
}

common::context_glfw::context_glfw() {
	if (spdlog::get("glfw") == nullptr) {
		logger = spdlog::stdout_color_mt("glfw");

		if constexpr (common::isDebug) {
			logger->set_level(spdlog::level::debug);
		}
	} else {
        logger = spdlog::get("glfw");
	}

	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) {
		logger->error("Can not init GLFW!");
		exit(EXIT_FAILURE);
	}
}

common::context_glfw::~context_glfw() {
	window.reset();
	glfwTerminate();
}

void common::context_glfw::create() {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, common::isDebug);
	glfwWindowHint(GLFW_VISIBLE, !common::headless);

	window = UniqueGLFWWindow(glfwCreateWindow(common::windowWidth, common::windowHeight, __func__, nullptr, nullptr));

	if (!window) {
		logger->error("Can not open window!");
		exit(EXIT_FAILURE);

    }

    int width, height;

    glfwGetFramebufferSize(window.get(), &width, &height);

    logger->info("Created window: {}x{} px", width, height);

	glfwMakeContextCurrent(window.get());
	glfwSwapInterval(0);
}

bool common::context_glfw::shouldClose() {
	return glfwWindowShouldClose(this->window.get());
}

void common::context_glfw::pollEvents() {
	glfwPollEvents();
}

glbinding::GetProcAddress common::context_glfw::getProcAddressPtr() {
	return glfwGetProcAddress;
}

void common::context_glfw::swapBuffers() {
	glfwSwapBuffers(window.get());
}
