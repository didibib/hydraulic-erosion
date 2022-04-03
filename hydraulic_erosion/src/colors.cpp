#include "hepch.h"
#include "vertex.h"
#include "colors.h"

namespace he
{
	ColorGenerator::ColorGenerator(std::vector<glm::vec4>& biomeColors, float spread)
	{
		m_biome_colors = biomeColors;
		m_spread = spread;
		m_half_spread = m_spread / 2.f;
		m_part = 1.f / (biomeColors.size() - 1);
	}

	void ColorGenerator::generateColors(std::vector<Vertex>& heightData, float amplitude)
	{
		for (int i = 0; i < heightData.size(); i++)
		{
			float height = heightData[i].position.z;
			heightData[i].color = calculateColor(height, amplitude);
		}
	}

	glm::vec4 ColorGenerator::calculateColor(float height, float amplitude)
	{
		float value = (height + amplitude) / (amplitude * 2);
		value = std::clamp((value - m_half_spread) * (1.f / m_spread), 0.f, 0.9999f);
		int firstBiome = (int)std::floor(value / m_part);
		float blend = (value - (firstBiome * m_part)) / m_part;
		return interpolateColors(m_biome_colors[firstBiome], m_biome_colors[firstBiome + 1], blend);
	}

	glm::vec4 ColorGenerator::interpolateColors(glm::vec4 color1, glm::vec4 color2, float blend)
	{
		float colorWeight = 1 - blend;
		float r = (colorWeight * color1.x) + (blend * color2.x);
		float g = (colorWeight * color1.y) + (blend * color2.y);
		float b = (colorWeight * color1.z) + (blend * color2.z);
		float a = (colorWeight * color1.a) + (blend * color2.a);
		return glm::vec4(r, g, b, a);
	}

}