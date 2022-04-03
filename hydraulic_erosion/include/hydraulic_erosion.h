#pragma once

#include "program.h"
#include "vertex.h"

namespace he
{
    class VertexBuffer;
    class Shader;

    struct DropletSettings
    {
        float inertia;   // [0,1], how much old_dir a droplet keeps after an update
        float carry_cap; // [0 ..], the total capacity of a droplet
        float depos_spd; // [0,1], the deposition speed of the sediment in a droplet
        float ero_spd;   // [0,1], the erosion speed of a droplet
        float evap_spd;  // [0,1], the evaporation speed of a droplet
        int ero_r;       // [0 ..], the erosion radius of a droplet
        float min_slope;
        float gravity;
    };

    class HydraulicErosion : public Program
    {
    public:
        HydraulicErosion();
        void init() override;
        void clear() override;
        void update(float) override;
        void draw(float) override;

    private:
        bool once = true;

        // heightmap
        std::vector<Vertex> m_height_data;
        VertexBuffer* m_terrain = nullptr;
        Shader* m_shader = nullptr;
        Shader* m_point_shader = nullptr;

        glm::vec2 m_grid_size;

        std::vector<Vertex> m_droplets;
		VertexBuffer* m_water = nullptr;
        int m_n_drops;                      // Keeps track of how many drops we have already simulated
        DropletSettings m_ds;

        const int m_MAX_DROPS = 100;
        const int m_DROPS_PER_ITER = 10;
        const int m_MAX_STEPS = 64;         // Maximum number of steps a droplet will take.

        bool m_draw_terrain = true;
        void drawTerrain(glm::mat4& pvMatrix, glm::mat4 model);
        void drawDroplets(glm::mat4& pvMatrix, glm::mat4 model);
        void simulateDroplet(float x, float y);
        void generateGrid(glm::vec2 size, float frequency = 1.f, float amplitude = 1.f, int octaves = 1);
	    // Bilinearly interpolates point within a quad
        float height(float _x, float _y);
        float height(int x, int y);
    };
}