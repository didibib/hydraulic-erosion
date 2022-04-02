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
		generateGrid(glm::vec2(512, 512), 2, 300, 5);

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
		m_terrain->draw(GL_TRIANGLES);
		m_terrain->unbind();

		m_shader->end();
	}

	void HydraulicErosion::generateGrid(glm::vec2 size, float frequency, float amplitude, int octaves)
	{
		int halfY = size.y / 2;
		int halfX = size.x / 2;
		std::vector<GLuint> indices;

		for (int y = 0; y < size.y; y++)
		{
			for (int x = 0; x < size.x; x++)
			{
				// Points
				float xfrac = x / size.x * frequency;
				float yfrac = y / size.y * frequency;
				float z = util::random::perlin.octave2D(xfrac, yfrac, octaves);

				Vertex v;
				v.position = glm::vec3(x - halfX, y - halfY, z * amplitude);
				//v.position = glm::vec3(x, y, 0);
				float color = (z + 1) / 2;
				v.color = glm::vec4(color, color, color, 1);
				m_height_data.push_back(v);
			}
		}

		for (int y = 0; y < size.y - 1; y++)
		{
			for (int x = 0; x < size.x - 1; x++)
			{
				// Indices
				int row1 = y * (size.y + 1);
				int row2 = (y + 1) * (size.y + 1);

				// triangle 1
				indices.push_back(row1 + x);
				indices.push_back(row1 + x + 1);
				indices.push_back(row2 + x + 1);

				// triangle 2
				indices.push_back(row1 + x);
				indices.push_back(row2 + x + 1);
				indices.push_back(row2 + x);
			}
		}

		m_terrain = new VertexBuffer("Terrain");
		m_terrain->init(m_height_data, indices);
	}
}