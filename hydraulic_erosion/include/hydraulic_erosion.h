#pragma once

#include "program.h"
#include "vertex.h"

namespace he
{
    class VertexBuffer;
    class Shader;

    class HydraulicErosion : public Program
    {
    public:
        HydraulicErosion();
        void init() override;
        void clear() override;
        void update(float) override;
        void draw(float) override;

    private:
        // heightmap
        std::vector<Vertex> m_height_data;
        VertexBuffer* m_terrain = nullptr;
        Shader* m_shader = nullptr;

        // Maximum number of steps a droplet will take.
        int m_max_steps;

        // Something something droplet functions
        void generateGrid(glm::vec2 size, float frequency = 1.f, float amplitude = 1.f, int octaves = 1);
    };
}