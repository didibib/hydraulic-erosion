#pragma once

#include "shader.h"

namespace he
{
	class BasicShader : public Shader
	{
	public:
		// @param filepath - Specify filepath withouth file type. We assume the filetypes: .vert, .frag
		bool load(const std::string& filepath) override;
	};
}