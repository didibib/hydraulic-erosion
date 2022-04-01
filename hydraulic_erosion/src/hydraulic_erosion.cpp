#include "hepch.h"
#include "hydraulic_erosion.h"
#include "vertex_buffer.h"
#include "shader.h"
#include "util.h"
#include "window.h"

namespace he
{
    HydraulicErosion::HydraulicErosion()
    {
    }

    void HydraulicErosion::init()
    {
        m_height_data = generateGrid(glm::vec2(1000, 1000));
        m_terrain = new VertexBuffer("Terrain");
        m_terrain->init(m_height_data);
        m_shader = new Shader(util::SHADER_DIR_STR + "basic");
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
        m_terrain->draw(GL_LINES);
        m_terrain->unbind();

        m_shader->end();
    }

    std::vector<Vertex> HydraulicErosion::generateGrid(glm::vec2 size)
    {
        std::vector<Vertex> grid;
        int halfY = size.y / 2;
        int halfX = size.x / 2;
        for (int y = -halfY; y < halfY; y++)
        {
            for (int x = -halfX; x < halfX; x++)
            {
                Vertex v;
                v.position = glm::vec3(x, y, 0);
                v.color = glm::vec4(0, 0, 0, 1);
                v.normal = glm::vec3(0, 0, 1);
                grid.push_back(v);
            }
        }
        return grid;
    }
}
