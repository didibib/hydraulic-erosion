#pragma once
 
#include "shader.h"

namespace he
{
	class TesselShader : public Shader
	{
	public:
		// @param filepath - Specify filepath withouth file type. We assume the filetypes: .vert, .frag, .tesctrl, .teseval
		bool load(const std::string& filepath) override;
	};
}