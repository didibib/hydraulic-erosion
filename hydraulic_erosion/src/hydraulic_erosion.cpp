#include "hepch.h"
#include "hydraulic_erosion.h"
#include "vertex_buffer.h"
#include "basic_shader.h"
#include "util.h"
#include "window.h"

namespace he
{
	HydraulicErosion::HydraulicErosion()
	{
	}

	void HydraulicErosion::init()
	{
		generateGrid(glm::vec2(1024, 1024), 2, 300, 5);
		
		m_shader = new BasicShader;
		m_shader->load(util::SHADER_DIR_STR + "basic");
	}

	void HydraulicErosion::clear()
	{
		auto clear_color = glm::vec4(245.f / 255.0f, 245.f / 255.0f, 220.f / 255.0f, 1.0f);
		glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void HydraulicErosion::update(float deltaTime)
	{
		// TODO perform erosion

		m_terrain->update(m_height_data);
	}

	void HydraulicErosion::draw(float deltaTime)
	{
		Camera& camera = Window::getCamera();

		auto projectionView = camera.getProjMatrix() * camera.getViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);

		m_shader->begin();

		m_shader->setMat4("u_ProjectionView", projectionView);
		m_shader->setMat4("u_Model", model);

		m_terrain->bind();
		m_terrain->draw(GL_TRIANGLE_STRIP);
		m_terrain->unbind();

		m_shader->end();
	}

	void HydraulicErosion::generateGrid(glm::vec2 size, float frequency, float amplitude, int octaves)
	{
		std::vector<unsigned int> indices;
		int halfY = size.y / 2;
		int halfX = size.x / 2;
		for (unsigned int y = 0; y < size.y; y++)
		{
			for (unsigned int x = 0; x < size.x; x++)
			{
				Vertex v;
				float xfrac = x / size.x * frequency;
				float yfrac = y / size.y * frequency;
				float z = util::random::perlin.octave2D(xfrac, yfrac, octaves);
				v.position = glm::vec3(x - halfX, y - halfY, z * amplitude);

				float color = (z + 1) / 2;
				v.color = glm::vec4(color, color, color, 1);
				m_height_data.push_back(v);

			/*	if (y >= size.y - 1)
					continue;*/

				for (unsigned int k = 0; k < 2; k++)      // for each side of the strip
				{
					indices.push_back(x + size.x * (y + k));
				}
			}
		}
		m_terrain = new VertexBuffer("Terrain");
		m_terrain->init(m_height_data, indices);
	}
}
