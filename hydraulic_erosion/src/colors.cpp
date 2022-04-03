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

	void ColorGenerator::generateColors(std::vector<Vertex>& heightData, glm::vec2 size, float amplitude)
	{
		for (int i = 0; i < heightData.size(); i++)
		{
			heightData[i].normal = glm::vec3(0);
		}

		for (int y = 0; y < size.y; y++)
		{
			for (int x = 0; x < size.x; x++)
			{
				int i1 = x + y * size.x;
				float height = heightData[i1].position.z;
				heightData[i1].color = calculateColor(height, amplitude);

				if (y >= size.y - 1 || x >= size.x - 1)
					continue;

				// Now we calculate the normal of the quad, and add it to the average of the connected vertices
				int i2 = x + (y + 1) * size.x;
				glm::vec3 v1 = heightData[i1 + 1].position - heightData[i1].position;
				glm::vec3 v2 = heightData[i2].position - heightData[i1].position;
				glm::vec3 normal = glm::normalize(glm::cross(v1, v2));

				heightData[i1].normal += normal;
				heightData[i1+1].normal += normal;
				heightData[i2].normal += normal;
				heightData[i2+1].normal += normal;
			}
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