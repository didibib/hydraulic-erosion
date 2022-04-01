#include "hepch.h"
#include "shader.h"
#include "util.h"

namespace he
{
	Shader::Shader(const std::string& filepath) : m_program_id(0)
	{
		load(filepath);
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_program_id);
	}

	bool Shader::load(const std::string& filepath)
	{
		std::string vert_file_loc = filepath + ".vert";
		std::string vert_shader = util::getFileContents(vert_file_loc.c_str());

		std::string frag_file_loc = filepath + ".frag";
		std::string frag_shader = util::getFileContents(frag_file_loc.c_str());

		TRACE("Loading Shader: {} -> {}, {}", filepath, vert_file_loc, frag_file_loc);

		GLuint vert_id = compileShader(GL_VERTEX_SHADER, vert_shader);
		GLuint frag_id = compileShader(GL_FRAGMENT_SHADER, frag_shader);

		if (!vert_id || !frag_id)
		{
			return false;
		}

		linkShader(vert_id, frag_id);

		return true;
	}

	GLuint Shader::compileShader(GLenum shader_type, const std::string& file) const
	{
		GLuint shader_id = glCreateShader(shader_type);

		const char* src = file.c_str();
		glShaderSource(shader_id, 1, &src, nullptr);

		glCompileShader(shader_id);

		GLint succes = 0;
		glGetShaderiv(shader_id, GL_COMPILE_STATUS, &succes);
		if (succes == GL_FALSE)
		{
			GLint max_length = 0;
			glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &max_length);

			// The max_length includes the NULL character
			std::vector<GLchar> error_log(max_length);
			glGetShaderInfoLog(shader_id, max_length, &max_length, &error_log[0]);

			// Provide the infolog in whatever manner you deem best.
			// Exit with failure.
			glDeleteShader(shader_id); // Don't leak the Shader.

			ERROR("Shader failed to compile\n{}", &(error_log[0]));
			ERROR("{}", file);
			return false;
		}
		return shader_id;
	}

	void Shader::linkShader(GLuint& vert_id, GLuint& frag_id)
	{
		m_program_id = glCreateProgram();

		// Attach our shaders to our program
		glAttachShader(m_program_id, vert_id);
		glAttachShader(m_program_id, frag_id);

		// Link our program
		glLinkProgram(m_program_id);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint succes(0);
		glGetProgramiv(m_program_id, GL_LINK_STATUS, &succes);
		if (!succes)
		{
			GLint max_length(0);
			glGetProgramiv(m_program_id, GL_INFO_LOG_LENGTH, &max_length);

			// The max_length includes the NULL character
			std::vector<GLchar> error_log(max_length);
			glGetProgramInfoLog(m_program_id, max_length, &max_length, &error_log[0]);

			// We don't need the program anymore.
			glDeleteProgram(m_program_id);
			// Don't leak shaders either.
			glDeleteShader(vert_id);
			glDeleteShader(frag_id);

			// Use the infoLog as you see fit.
			ERROR("Shader failed to link");
			ERROR(&(error_log[0]));

			return;
		}

		// Always detach and delete shaders after a successful link.
		glDetachShader(m_program_id, vert_id);
		glDetachShader(m_program_id, frag_id);
		glDeleteShader(vert_id);
		glDeleteShader(frag_id);
	}

	void Shader::begin() const
	{
		glUseProgram(m_program_id);
	}

	void Shader::end() const
	{
		glUseProgram(0);
	}

	GLuint Shader::getUniformLocation(const std::string& name) const
	{
		GLuint location = glGetUniformLocation(m_program_id, name.c_str());
		if (location == GL_INVALID_INDEX)
		{
			ERROR("Uniform variable not found in shader: {}", name);
		}
		return location;
	}

	void Shader::setInt(const char* name, const int& value) const
	{
		glUniform1i(getUniformLocation(name), value);
	}

	void Shader::setFloat(const char* name, const float& value) const
	{
		glUniform1f(getUniformLocation(name), value);
	}

	void Shader::setVec2(const char* name, const float& x, const float& y) const
	{
		glUniform2f(getUniformLocation(name), x, y);
	}

	void Shader::setVec2(const char* name, const glm::vec2& vector) const
	{
		glUniform2fv(getUniformLocation(name), 1, &vector[0]);
	}

	void Shader::setVec3(const char* name, const float& x, const float& y, const float& z) const
	{
		glUniform3f(getUniformLocation(name), x, y, z);
	}

	void Shader::setVec3(const char* name, const glm::vec3& vector) const
	{
		glUniform3fv(getUniformLocation(name), 1, &vector[0]);
	}

	void Shader::setVec4(const char* name, const glm::vec4& vector) const
	{
		glUniform4fv(getUniformLocation(name), 1, &vector[0]);
	}

	void Shader::setVec4(const char* name, const float& x, const float& y, const float& z, const float& w) const
	{
		glUniform4f(getUniformLocation(name), x, y, z, w);
	}

	void Shader::setMat4(const char* name, const glm::mat4& matrix) const
	{
		glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
	}
}