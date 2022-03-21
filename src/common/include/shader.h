#ifndef COMMON_SHADER_H
#define COMMON_SHADER_H

#include <cstdint>
#include <string>
#include <vector>

#include <glbinding/gl46core/gl.h>

namespace common {
	typedef struct {
		std::vector<gl::GLuint> indexes;
		std::vector<gl::GLuint> values;
	} Specialization;

	std::vector<std::uint8_t> loadShaderSpirv(const std::string &shaderFilename);

	std::vector<gl::GLchar> loadShaderGlsl(const std::string &shaderFilename);

	gl::GLuint loadCompileShader(gl::GLenum shaderType, const std::string &shaderFilename, const Specialization *spec = nullptr);

	gl::GLuint loadCompileShaderSpirv(gl::GLenum shaderType, const std::string &shaderFilename, const Specialization *spec = nullptr);

	gl::GLuint loadCompileShaderGlsl(gl::GLenum shaderType, const std::string &shaderFilename, const Specialization *spec = nullptr);

	gl::GLuint createProgram(const std::initializer_list<gl::GLuint> &shaderVector, const char *feedback = nullptr);

	void programInfo(gl::GLuint program);
}// namespace common

#endif//COMMON_SHADER_H
