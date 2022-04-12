#include "hepch.h"
#include "shallow_erosion.h"
#include "vertex_buffer.h"
#include "basic_shader.h"
#include "util.h"
#include "window.h"
#include "colors.h"

namespace he
{
	ShallowErosion::ShallowErosion()
	{
	}

	ShallowErosion::~ShallowErosion()
	{
		delete m_color_gen;
		delete m_terrain;
		delete m_water;
		delete m_shader;
		delete m_point_shader;
	}

	void ShallowErosion::init()
	{
		m_grid_size = glm::vec2(256);
		generateGrid(m_grid_size, 3, 80, 4);
		
		m_light_pos = glm::vec3(0, 0, 200);

		m_shader = new BasicShader;
		m_shader->load(util::SHADER_DIR_STR + "basic");

		m_water_shader = new BasicShader;
		m_water_shader->load(util::SHADER_DIR_STR + "water");

		m_point_shader = new BasicShader;
		m_point_shader->load(util::SHADER_DIR_STR + "point");

		// Colors
		m_biome_colors.push_back({ 201.f / 255.f, 178.f / 255.f, 99.f / 255.f, 1.0f });
		m_biome_colors.push_back({ 135.f / 255.f, 184.f / 255.f, 82.f / 255.f, 1.0f });
		m_biome_colors.push_back({ 80.f / 255.f, 171.f / 255.f, 93.f / 255.f, 1.0f });
		m_biome_colors.push_back({ 120.f / 255.f, 120.f / 255.f, 120.f / 255.f, 1.0f });
		m_biome_colors.push_back({ 200.f / 255.f, 200.f / 255.f, 210.f / 255.f, 1.0f });
		m_color_gen = new ColorGenerator(m_biome_colors, 0.5f);

		m_iterations = 0;
	}

	void ShallowErosion::clear()
	{
		auto clear_color = glm::vec4(245.f / 255.0f, 245.f / 255.0f, 220.f / 255.0f, 1.0f);
		glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void ShallowErosion::update(float deltaTime)
	{
		if (Window::isKeyPressed(GLFW_KEY_T))
			m_draw_terrain = !m_draw_terrain;
		if (Window::isKeyPressed(GLFW_KEY_C))
			m_draw_water = !m_draw_water;

		INFO("Iteration: {}", m_iterations++);

		deltaTime = 0.1;
		incrementWater(deltaTime);
		updateFlow(deltaTime);
		calculateSoilFlow(deltaTime);
		erodeAndDeposit(deltaTime);
		transportSediment(deltaTime);
		applyThermalErosion();
		evaporateWater(deltaTime);
		updateVertexHeight();

		INFO("Height at i 4000: {}", m_grid_data[4000].b);

		m_color_gen->generateColors(m_height_data, m_grid_size, 40);
		m_terrain->update(m_height_data);
		m_water->update(m_water_data);
	}

	void ShallowErosion::draw(float deltaTime)
	{
		Camera& camera = Window::getCamera();

		auto projectionView = camera.getProjMatrix() * camera.getViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);

		if (m_draw_terrain)
			drawTerrain(projectionView, model);
		if (m_draw_water)
			drawWater(projectionView, model);
	}

	void ShallowErosion::drawTerrain(glm::mat4& pvMatrix, glm::mat4 model)
	{
		m_shader->begin();

		//glm::mat4 normalMatrix = glm::transpose(glm::inverse(model));

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

	void ShallowErosion::drawWater(glm::mat4& pvMatrix, glm::mat4 model)
	{
		m_water_shader->begin();

		m_water_shader->setMat4("u_ProjectionView", pvMatrix);
		m_water_shader->setMat4("u_Model", model);

		m_water->bind();
		m_water->draw(m_draw_mode);
		m_water->unbind();

		m_water_shader->end();
	}


	void ShallowErosion::generateGrid(glm::vec2 size, float frequency, float amplitude, int octaves)
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

				Vertex v2;
				v2.position = glm::vec3(x, y, 0);
				v2.color = glm::vec4(0, 0, 1, 0.5);
				m_water_data.push_back(v2);

				GridPoint p(z * amplitude, 0.f, 0.f, std::vector<float>{0.f, 0.f, 0.f, 0.f}, std::vector<float>{}, glm::vec2(0.f, 0.f), 1, 1);
				m_grid_data.push_back(p);
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

		delete m_terrain;
		m_terrain = new VertexBuffer("Terrain");
		m_terrain->init(m_height_data, indices);

		delete m_water;
		m_water = new VertexBuffer("Water");
		m_water->init(m_water_data, indices);
	}

	void ShallowErosion::incrementWater(float deltaTime)
	{
		for(int i = 0; i < m_grid_data.size(); i++)
		{
			auto& g = m_grid_data[i];
			g.d = g.d + deltaTime * g.r * m_gp.rain_rate;
		}
	}

	void ShallowErosion::updateFlow(float deltaTime)
	{
		// For each cell
		// Calculate the outgoing flow
		for (int i = 0; i < m_grid_data.size(); i++)
		{
			auto& g = m_grid_data[i];
			std::vector<float> flows{ 0.f, 0.f, 0.f, 0.f };

			// Check 4 adjacent cells and calculate flows
			for (int j = 0; j < 4; j++)
			{
				auto newPos = getNewPos(i, m_directions[j]);
				if (!newPos) continue;

				auto& gD = m_grid_data[newPos->x + newPos->y * m_grid_size.x];
				float hdiff = g.b + g.d - gD.b - gD.d;
				float flow = g.flux[j] + deltaTime * m_gp.cross_section_area * m_gp.gravity * hdiff;
				flows[j] = std::max(0.f, flow);
			}

			// Make sure that we don't flow more water than the cell contains
			float flowsTotal = 0;
			for (int j = 0; j < flows.size(); j++) flowsTotal += flows[j];
			float kFactor = glm::min(1.f, g.d / flowsTotal);
			//float kFactor = glm::max(1.f, g.d / (flowsTotal*deltaTime));
			// Update the flux of the grid points
			for (int j = 0; j < flows.size(); j++) g.flux[j] = flows[j] * kFactor;
		}

		// For each cell, again,
		// Calculate the Delta V water height changes and velocity
		for (int i = 0; i < m_grid_data.size(); i++)
		{
			auto& g = m_grid_data[i];
			float deltaV = 0.f;
			std::vector<float> incFlux{ 0.f, 0.f, 0.f, 0.f };

			// Check adjacent cells and sum in/outgoing flows
			for (int j = 0; j < 4; j++)
			{
				auto newPos = getNewPos(i, m_directions[j]);
				if (!newPos) continue;

				auto& gD = m_grid_data[newPos->x + newPos->y * m_grid_size.x];
				incFlux[j] = gD.flux[(j + 2) % 4];
			}

			// Subtract outgoing flux
			for (int j = 0; j < g.flux.size(); j++)
			{
				deltaV -= g.flux[j];
				deltaV += incFlux[j];
			}

			g.d += deltaV * deltaTime;

			float wX = 0.5f * (incFlux[0] - g.flux[0] + g.flux[2] - incFlux[2]);
			float wY = 0.5f * (incFlux[1] - g.flux[1] + g.flux[3] - incFlux[3]);

			g.v = glm::vec2(wX, wY);
		}
	}

	void ShallowErosion::erodeAndDeposit(float deltaTime)
	{
		for (int i = 0; i < m_grid_data.size(); i++)
		{
			auto& g = m_grid_data[i];

			float C = m_gp.sediment_capacity * std::min(m_gp.min_local_tilt_angle, localTiltAngle(i)) * glm::length(g.v) * shallowDeepLimiter(g.d);

			if (g.s < C)
			{
				float soil = deltaTime * g.R * m_gp.soil_suspension_rate * (C - g.s);
				g.b = g.b - soil;
				g.s = g.s + soil;
				g.d = g.d + soil;

			}
			else
			{
				float soil = deltaTime * m_gp.sediment_depo_rate * (g.s - C);
				g.b = g.b + soil;
				g.s = g.s - soil;
				g.d = g.d - soil;
				g.R = std::max(m_gp.hardness_lower_limit, g.R - deltaTime * m_gp.sediment_soft_rate * m_gp.soil_suspension_rate * (g.s - C));
			}
		}
	}
	
	// https://math.stackexchange.com/questions/1044044/local-tilt-angle-based-on-height-field
	float ShallowErosion::localTiltAngle(int index)
	{
		auto& g = m_grid_data[index];

		// Heights of neighbours
		std::vector<float> nh{ 0.f, 0.f, 0.f, 0.f };
		for (int i = 0; i < 4; i++)
		{
			auto newPos = getNewPos(index, m_directions[i]);
			if (!newPos)
			{
				// Out-of-bounds, so the height stays the same
				nh[i] = g.b;
				continue;
			}
			nh[i] = m_grid_data[newPos->x + newPos->y * m_grid_size.x].b;
		}

		float dhdx = (nh[2] - nh[0]) / 2;
		float dhdy = (nh[3] - nh[1]) / 2;

		return sqrtf(dhdx * dhdx + dhdy * dhdy) / sqrtf(1 + dhdx * dhdx + dhdy * dhdy);
	}

	float ShallowErosion::shallowDeepLimiter(float d)
	{
		if (d < 0)
			return 0.f;
		if (d > m_gp.max_erosion_depth)
			return 1.f;
		return (1 - m_gp.max_erosion_depth + d) / m_gp.max_erosion_depth;
	}

	std::optional<glm::ivec2> ShallowErosion::getNewPos(int index, glm::ivec2 dir)
	{
		glm::ivec2 pos(index % int(m_grid_size.x), index / int(m_grid_size.x));
		glm::ivec2 dPos = pos + dir;

		if (dPos.x < 0 || dPos.x >= m_grid_size.x || dPos.y < 0 || dPos.y >= m_grid_size.y)
		{
			return std::nullopt;
		}
		return std::optional<glm::ivec2>(dPos);
	}

	void ShallowErosion::transportSediment(float deltaTime)
	{
		for (int i = 0; i < m_grid_data.size(); i++)
		{
			auto& g = m_grid_data[i];

			glm::vec2 pos = glm::vec2(i % int(m_grid_size.x) - g.v.x * deltaTime, i / int(m_grid_size.x) - g.v.y * deltaTime);

			int x = floor(pos.x);
			int y = floor(pos.y);
			float u = pos.x - x;
			float v = pos.y - y;

			float diff1 = sediment(x + 1, y) * u + sediment(x, y) * (1 - u);
			float diff2 = sediment(x + 1, y + 1) * u + sediment(x, y + 1) * (1 - u);

			g.s = diff1 * (1 - v) + diff2 * v;
		}
	}

	float ShallowErosion::sediment(int x, int y)
	{
		if (x < 0 || x >= m_grid_size.x || y < 0 || y >= m_grid_size.y)
		{
			return 0;
		}
		return m_grid_data[x + y * m_grid_size.x].s;
	}

	void ShallowErosion::evaporateWater(float deltaTime)
	{
		for (int i = 0; i < m_grid_data.size(); i++)
		{
			auto& g = m_grid_data[i];
			g.d = g.d * (1 - m_gp.water_evap_rate * deltaTime);
		}
	}

	void ShallowErosion::updateVertexHeight()
	{
		for (int i = 0; i < m_grid_data.size(); i++)
		{
			m_height_data[i].position.z = m_grid_data[i].b;
			m_water_data[i].position.z = m_grid_data[i].b + m_grid_data[i].d - m_gp.water_z_offset;
		}
	}
	 
	void ShallowErosion::calculateSoilFlow(float deltaTime)
	{
		for (int i = 0; i < m_grid_data.size(); i++)
		{
			auto& g = m_grid_data[i];
			float H = 0; // Max height
			std::vector<int> A; // Set of neighbours that lie below the talus angle
			float sumA = 0;

			for (int j = 0; j < 8; j++)
			{
				auto newPos = getNewPos(i, m_directions[j]);
				if (!newPos) continue;
				auto& gi = m_grid_data[newPos->x + newPos->y * m_grid_size.x];
				H = std::max(H, g.b - gi.b);

				float d = 1;
				if (j >= 4)
					d = sqrtf(2);

				float alpha = glm::tan((g.b - gi.b) / d);
				if (g.b - gi.b < 0 && glm::tan(alpha) > g.R * m_gp.talus_angle_tang_coeff + m_gp.talus_angle_tang_bias)
				{
					A.push_back(j);
					sumA += gi.b;
				}
			}
			float deltaS = deltaTime * m_gp.thermal_erosion_rate * g.R * H / 2;

			for (int j = 0; j < A.size(); j++)
			{
				auto newPos = getNewPos(i, m_directions[j]);
				if (!newPos) continue;
				auto& gi = m_grid_data[newPos->x + newPos->y * m_grid_size.x];

				//int oppositeDir = 0;
				//if (j < 4)
				//	oppositeDir = (j + 2) % 4;
				//else
				//	oppositeDir = ((j - 2) % 4) + 4;

				float soil = deltaS * gi.b / sumA;
				gi.pipes.push_back(soil);

			}
			g.pipes.push_back(-deltaS);
		}
	}

	void ShallowErosion::applyThermalErosion()
	{
		for (int i = 0; i < m_grid_data.size(); i++)
		{
			auto& g = m_grid_data[i];

			for (int j = 0; j < g.pipes.size(); j++)
			{
				g.b += g.pipes[j];
				g.pipes[j] = 0;
				g.pipes.clear();
			}
		}
	}
}