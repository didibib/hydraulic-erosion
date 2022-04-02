#include "hepch.h"
#include "tessel_shader.h"
#include "util.h"

namespace he
{
	bool TesselShader::load(const std::string& filepath)
	{
		std::string vert_file_loc = filepath + ".vert";
		std::string vert_shader = util::getFileContents(vert_file_loc.c_str());

		std::string tess_ctrl_file_loc = filepath + ".tessctrl";
		std::string tess_ctrl_shader = util::getFileContents(tess_ctrl_file_loc.c_str());

		std::string tess_eval_file_loc = filepath + ".tesseval";
		std::string tess_eval_shader = util::getFileContents(tess_eval_file_loc.c_str());

		std::string frag_file_loc = filepath + ".frag";
		std::string frag_shader = util::getFileContents(frag_file_loc.c_str());

		TRACE("Loading Shader: {} -> {}, {}, {}, {}", filepath, vert_file_loc, tess_ctrl_file_loc, tess_eval_file_loc, frag_file_loc);

		GLuint vert_id = compileShader(GL_VERTEX_SHADER, vert_shader);
		GLuint tess_ctrl_id = compileShader(GL_TESS_CONTROL_SHADER, tess_ctrl_shader);
		GLuint tess_eval_id = compileShader(GL_TESS_EVALUATION_SHADER, tess_eval_shader);
		GLuint frag_id = compileShader(GL_FRAGMENT_SHADER, frag_shader);

		if (!vert_id || !frag_id || !tess_ctrl_id || !tess_eval_id)
		{
			return false;
		}

		std::vector<GLuint> ids{ vert_id, tess_ctrl_id, tess_eval_id, frag_id };
		linkShaders(ids);
		return true;
	}
}
