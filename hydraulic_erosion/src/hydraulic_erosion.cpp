#include "hepch.h"
#include "hydraulic_erosion.h"
#include "vertex_buffer.h"
#include "basic_shader.h"
#include "util.h"
#include "window.h"
#include "colors.h"

namespace he
{
	HydraulicErosion::HydraulicErosion()
	{
	}

	HydraulicErosion::~HydraulicErosion()
	{
		delete m_color_gen;
		delete m_terrain;
		delete m_shader;
		delete m_point_shader;
		delete m_water;
	}

	void HydraulicErosion::init()
	{
		m_grid_size = glm::vec2(256, 256);
		generateGrid(m_grid_size, 3, 40, 4);

		m_water = new VertexBuffer("chwater");
		m_droplets.resize(1);
		m_water->init(m_droplets);

		m_shader = new BasicShader;
		m_shader->load(util::SHADER_DIR_STR + "basic");

		m_point_shader = new BasicShader;
		m_point_shader->load(util::SHADER_DIR_STR + "point");

		m_ds.inertia = .04f;
		m_ds.p_capacity = 8.f;
		m_ds.p_deposit = .3f;
		m_ds.p_erosion = .1f;
		m_ds.p_evaporation = .04f;
		m_ds.min_slope = .01f;
		m_ds.gravity = 1.f;

		// Colors
		m_biome_colors.push_back({ 201.f / 255.f, 178.f / 255.f, 99.f / 255.f, 1.0f });
		m_biome_colors.push_back({ 135.f / 255.f, 184.f / 255.f, 82.f / 255.f, 1.0f });
		m_biome_colors.push_back({ 80.f / 255.f, 171.f / 255.f, 93.f / 255.f, 1.0f });
		m_biome_colors.push_back({ 120.f / 255.f, 120.f / 255.f, 120.f / 255.f, 1.0f });
		m_biome_colors.push_back({ 200.f / 255.f, 200.f / 255.f, 210.f / 255.f, 1.0f });
		m_color_gen = new ColorGenerator(m_biome_colors, 0.5f);

		m_light_pos = glm::vec3(0, 0, 200);
	}

	void HydraulicErosion::clear()
	{
		auto clear_color = glm::vec4(245.f / 255.0f, 245.f / 255.0f, 220.f / 255.0f, 1.0f);
		glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void HydraulicErosion::update(float deltaTime)
	{
		if (Window::isKeyPressed(GLFW_KEY_I))
			m_draw_mode = GL_TRIANGLES;
		if (Window::isKeyPressed(GLFW_KEY_O))
			m_draw_mode = GL_LINES;
		if (Window::isKeyPressed(GLFW_KEY_T))
			m_draw_terrain = !m_draw_terrain;
		if (Window::isKeyPressed(GLFW_KEY_U))
		{
			m_add_droplets = !m_add_droplets;
			if (!m_add_droplets)
			{
				m_droplets.clear();
				m_droplets.resize(1);
			}
		}


		for (int i = 0; i < m_DROPS_PER_ITER; i++)
		{
			if (m_n_drops > m_MAX_DROPS)
				break;
			// Create a new drop
			float x = util::random::Range(m_grid_size.x - 1);
			float y = util::random::Range(m_grid_size.y - 1);

			// Simulate the drop
			simulateDroplet(x, y);
			m_n_drops++;
			
		}

		m_color_gen->generateColors(m_height_data, m_grid_size, 40);
		m_terrain->update(m_height_data);
		m_water->update(m_droplets);
	}

	void HydraulicErosion::simulateDroplet(float startX, float startY)
	{
		int x, y;
		float u, v;
		Droplet d;
		d.pos = glm::vec2(startX, startY);
		d.dir = glm::normalize(glm::vec2(util::random::Range(1), util::random::Range(1)));
		d.sediment = 0;
		d.water = 10;
		d.vel = 0;
		d.radius = 2;

		for (int i = 0; i < m_MAX_STEPS; i++)
		{
			if (m_add_droplets)
			{
				Vertex vertex;
				vertex.position = glm::vec3(d.pos, height(d.pos.x, d.pos.y));
				vertex.color = glm::vec4(0, 0, (1.f / i), 1);
				m_droplets.push_back(vertex);
			}

			// We work under the assumption that we are still inside a quad, otherwise we would have stopped in the previous step
			x = floor(d.pos.x);
			y = floor(d.pos.y);
			u = d.pos.x - x;
			v = d.pos.y - y;

			// Calculate the gradient of our old position
			glm::vec2 g;
			g.x = (height(x + 1, y) - height(x, y)) * (1 - v) + (height(x + 1, y + 1) - height(x, y + 1)) * v;
			g.y = (height(x, y + 1) - height(x, y)) * (1 - u) + (height(x + 1, y + 1) - height(x + 1, y)) * u;
			g = glm::normalize(g);

			d.dir = d.dir * m_ds.inertia - g * (1 - m_ds.inertia);

			float hOld = height_bli(d.pos.x, d.pos.y);
			glm::vec2 oldPos = d.pos;
			d.pos = oldPos + d.dir;

			// Check if new position is within the bounds of the grid
			if (d.pos.x < 0 || d.pos.x >= m_grid_size.x - 1
				|| d.pos.y < 0 || d.pos.y >= m_grid_size.y - 1)
				break;

			float hNew = height_bli(d.pos.x, d.pos.y);
			float hDiff = hNew - hOld;

			if (hDiff > 0)
			{
				// Droplet is moving upwards, so it needs to drop sediment to fill the hole behind us
				distribute(d, oldPos, std::min(hDiff, d.sediment));
			}
			else
			{
				float capacity = std::max(-hDiff, m_ds.min_slope) * d.vel * d.water * m_ds.p_capacity;

				if (d.sediment > capacity)
				{
					float surplus = (d.sediment - capacity) * m_ds.p_deposit;
					distribute(d, oldPos, surplus);
				}
				else
				{
					float amount = std::min((capacity - d.sediment) * m_ds.p_erosion, -hDiff);
					erode(d, oldPos, amount);
				}
			}

			d.vel = sqrt(d.vel * d.vel + std::abs(hDiff) * m_ds.gravity);
			d.water = d.water * (1 - m_ds.p_evaporation);
		}
	}

	void HydraulicErosion::distribute(Droplet& droplet, glm::vec2 pos, float sediment)
	{
		droplet.sediment -= sediment;
		// Use bilinear interpolation to distribute the sediment over the four adjacent points.
		int x = floor(pos.x);
		int y = floor(pos.y);
		float u = pos.x - x;
		float v = pos.y - y;

		// d1 -u- d2
		// |      |
		// v   p  |
		// |      |
		// d3 --- d4
		// p(x + u, y + v)
		// d1(x, y)
		float d1 = 1 / sqrt(u * u + v * v);
		float d2 = 1 / sqrt((1 - u) * (1 - u) + v * v);
		float d3 = 1 / sqrt(u * u + (1 - v) * (1 - v));
		float d4 = 1 / sqrt((1 - u) * (1 - u) + (1 - v) * (1 - v));
		float dTotal = d1 + d2 + d3 + d4;

		addHeight(x/**/, y/**/, sediment * (d1 / dTotal));
		addHeight(x + 1, y/**/, sediment * (d2 / dTotal));
		addHeight(x/**/, y + 1, sediment * (d3 / dTotal));
		addHeight(x + 1, y + 1, sediment * (d4 / dTotal));
	}

	void HydraulicErosion::erode(Droplet& d, glm::vec2 pos, float amount)
	{
		std::vector<float> weights;
		std::vector<glm::vec2> positions;
		float weightSum = 0;

		int xFloor = floor(pos.x);
		int yFloor = floor(pos.y);

		for (int y = yFloor - d.radius; y <= yFloor + d.radius; y++)
		{
			for (int x = xFloor - d.radius; x <= xFloor + d.radius; x++)
			{
				glm::vec2 p(x, y);
				float dist = glm::length(p - pos);
				if (dist > d.radius)
					continue;

				float wi = std::max(0.f, d.radius - dist);
				weightSum += wi;
				weights.push_back(wi);
				positions.push_back(p);
			}
		}

		for (int i = 0; i < weights.size(); i++)
		{
			float wiNorm = weights[i] / weightSum;
			glm::vec2 p = positions[i];

			if (p.x < 0 || p.x >= m_grid_size.x || p.y < 0 || p.y >= m_grid_size.y)
				continue;

			addHeight((int)p.x, (int)p.y, -wiNorm * amount);
		}

		d.sediment += amount;
	}

	float HydraulicErosion::height_bli(float _x, float _y)
	{
		int x = floor(_x);
		int y = floor(_y);
		float u = _x - x;
		float v = _y - y;

		float diff1 = height(x + 1, y) * u + height(x, y) * (1 - u);
		float diff2 = height(x + 1, y + 1) * u + height(x, y + 1) * (1 - u);

		return diff1 * (1 - v) + diff2 * v;
	}

	float HydraulicErosion::height(int x, int y)
	{
		int index = x + y * m_grid_size.x;
		return m_height_data[index].position.z;
	}

	void HydraulicErosion::addHeight(int x, int y, float height)
	{
		int index = x + y * m_grid_size.x;
		m_height_data[index].position.z += height;
	}

	void HydraulicErosion::draw(float deltaTime)
	{
		Camera& camera = Window::getCamera();

		auto projectionView = camera.getProjMatrix() * camera.getViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);

		if (m_draw_terrain)
			drawTerrain(projectionView, model);
		drawDroplets(projectionView, model);
	}

	void HydraulicErosion::drawTerrain(glm::mat4& pvMatrix, glm::mat4 model)
	{
		m_shader->begin();

		glm::mat4 normalMatrix = glm::transpose(glm::inverse(model));

		m_shader->setMat4("u_ProjectionView", pvMatrix);
		m_shader->setMat4("u_Model", model);
		//m_shader->setMat4("u_NormalMatrix", normalMatrix);
		m_shader->setVec3("u_LightPos", m_light_pos);
		m_shader->setVec3("u_LightColor", glm::vec3(1, 1, 1));

		m_terrain->bind();
		m_terrain->draw(m_draw_mode);
		m_terrain->unbind();

		m_shader->end();
	}

	void HydraulicErosion::drawDroplets(glm::mat4& pvMatrix, glm::mat4 model)
	{
		m_point_shader->begin();
		m_point_shader->setMat4("u_ProjectionView", pvMatrix);
		m_point_shader->setMat4("u_Model", model);

		m_water->bind();
		m_water->draw(GL_POINTS);
		m_water->unbind();

		m_point_shader->end();
	}

	void HydraulicErosion::generateGrid(glm::vec2 size, float frequency, float amplitude, int octaves)
	{
		std::vector<GLuint> indices;

		for (int y = 0; y < size.y; y++)
		{
			for (int x = 0; x < size.x; x++)
			{
				// Points
				float xfrac = x / size.x * frequency;
				float yfrac = y / size.y * frequency;
				float z = util::random::perlin.octave2D(xfrac + 1, yfrac + 1, octaves);

				Vertex v;
				v.position = glm::vec3(x, y, z * amplitude);
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