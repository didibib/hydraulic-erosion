#include "hepch.h"
#include "basic_shader.h"
#include "util.h"

namespace he
{
	bool BasicShader::load(const std::string& filepath)
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
		
		std::vector<GLuint> ids{ vert_id, frag_id };
		linkShaders(ids);

		return true;
	}
}
