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
		m_grid_size = glm::vec2(512, 512);
		generateGrid(m_grid_size, 3, 80, 8);

		m_water = new VertexBuffer("chwater");
		m_droplets.resize(1);
		m_water->init(m_droplets);

		m_shader = new BasicShader;
		m_shader->load(util::SHADER_DIR_STR + "basic");

		m_point_shader = new BasicShader;
		m_point_shader->load(util::SHADER_DIR_STR + "point");
	}

	void HydraulicErosion::clear()
	{
		auto clear_color = glm::vec4(245.f / 255.0f, 245.f / 255.0f, 220.f / 255.0f, 1.0f);
		glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void HydraulicErosion::update(float deltaTime)
	{
		// Simulate a certain amount of droplets before we update our terrain
		for (int i = 0; i < m_DROPS_PER_ITER; i++)
		{
			if (m_n_drops++ < m_MAX_DROPS)
				break;

			// Create a new drop
			float x = util::random::Range(m_grid_size.x);
			float y = util::random::Range(m_grid_size.y);

			// Simulate the drop
			simulateDroplet(x, y);
		}

		// Perform thermal erosion

		m_terrain->update(m_height_data);
		m_water->update(m_droplets);
	}

	void HydraulicErosion::simulateDroplet(float startX, float startY)
	{
		int x, y;
		float u, v;
		int cur_steps;
		glm::vec2 pos;
		glm::vec2 dir;
		float vel, water, sediment;

		for (int i = 0; i < m_MAX_STEPS; i++)
		{
			Vertex vertex;
			vertex.position = glm::vec3(pos, height(pos.x, pos.y));
			vertex.color = glm::vec4(1, 0, 0, 1);
			m_droplets.push_back(vertex);

			// We work under the assumption that we are still inside a quad, otherwise we would have stopped in the previous step
			x = floor(pos.x);
			y = floor(pos.y);
			u = pos.x - x;
			v = pos.y - y;

			// Calculate the gradient of our old position
			glm::vec2 g;
			g.x = (height(x + 1, y) - height(x, y)) * (1 - v) + (height(x + 1, y + 1) - height(x, y + 1)) * v;
			g.y = (height(x, y + 1) - height(x, y)) * (1 - u) + (height(x + 1, y + 1) - height(x + 1, y)) * u;
			g = glm::normalize(g);

			dir = dir * m_ds.inertia - g * (1 - m_ds.inertia);

			float hOld = height(pos.x, pos.y);
			pos = pos + dir;
			float hNew = height(pos.x, pos.y);
			float hDiff = hNew - hOld;

			if (hDiff > 0)
			{

			}
			else
			{

			}
		}
	}

	float HydraulicErosion::height(float _x, float _y)
	{
		int x = floor(_x);
		int y = floor(_y);
		float u = _x - x;
		float v = _y - y;

		float diff1 = height(x + 1, y) * u + height(x, y) * (1 - u);
		float diff2 = height(x + 1, y + 1) * u + height(x, y + 1) * (1 - u);

		return diff1 * (1-v) + diff2 * v;
	}

	float HydraulicErosion::height(int x, int y)
	{
		int index = x + y * m_grid_size.x;
		return m_height_data[index].position.z;
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
		m_terrain->draw();
		m_terrain->unbind();

		m_shader->end();

		m_point_shader->begin();
		m_point_shader->setMat4("u_ProjectionView", projectionView);
		m_point_shader->setMat4("u_Model", model);

		m_water->bind();
		m_water->draw();
		m_water->unbind();

		m_point_shader->end();
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
				int row1 = y * (size.x);
				int row2 = row1 + size.x;

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