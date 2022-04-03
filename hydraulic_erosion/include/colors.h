#pragma once

namespace he
{
	// https://github.com/TheThinMatrix/LowPolyTerrain/blob/master/src/generation/ColourGenerator.java
	class ColorGenerator
	{
	public:
		ColorGenerator(std::vector<glm::vec4>& biomeColors, float spread);
		void generateColors(std::vector<Vertex>& heightData, glm::vec2 size, float amplitude);
	private:
		std::vector<glm::vec4> m_biome_colors;
		float m_spread;
		float m_half_spread;
		float m_part;

		glm::vec4 calculateColor(float height, float amplitude);
		// https://github.com/TheThinMatrix/LowPolyTerrain/blob/master/OpenGlWrapper/utils/Colour.java
		glm::vec4 interpolateColors(glm::vec4, glm::vec4, float blend);
	};
}