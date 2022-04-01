#pragma once

namespace he
{
	class Shader
	{
	public:
		Shader(const std::string& filepath);
		~Shader();
		// @param filepath - Specify filepath withouth file type. We assume the filetypes: .vert, .frag
		bool load(const std::string& filepath);
		void begin() const;
		void end() const;

		GLuint getUniformLocation(const std::string& name) const;
		void setInt(const char* name, const int& value) const;
		void setFloat(const char* name, const float& value) const;
		void setVec2(const char* name, const glm::vec2& vector) const;
		void setVec2(const char* name, const float& x, const float& y) const;
		void setVec3(const char* name, const glm::vec3& vector) const;
		void setVec3(const char* name, const float& x, const float& y, const float& z) const;
		void setVec4(const char* name, const glm::vec4& vector) const;
		void setVec4(const char* name, const float& x, const float& y, const float& z, const float& w) const;
		void setMat4(const char* name, const glm::mat4& matrix) const;

	private:
		GLuint compileShader(GLenum shader_type, const std::string& file)  const;
		void linkShader(GLuint& vert_id, GLuint& frag_id);

		GLuint m_program_id;
	};
}