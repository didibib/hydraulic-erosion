#pragma once

namespace he
{
	struct Vertex
	{
		glm::vec3 position;		// 3 * float = 12 bytes
		glm::vec4 color;		// 4 * float = 16 bytes
		glm::vec2 uv;			// 2 * float = 8  bytes
		glm::vec3 normal;		// 3 * float = 12 bytes
	};
}