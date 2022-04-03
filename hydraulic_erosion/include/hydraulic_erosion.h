#pragma once

#include "program.h"
#include "vertex.h"

namespace he
{
    class VertexBuffer;
    class Shader;
    class ColorGenerator;

    struct DropletSettings
    {
        float inertia;          // [0,1], how much old_dir a droplet keeps after an update
        float p_capacity;       // [0,n], the total capacity of a droplet
        float p_deposit;        // [0,1], deposition speed
        float p_erosion;        // [0,1], erosion speed
        float p_evaporation;    // [0,1], evaporation speed
        float min_slope;
        float gravity;
    };

    struct Droplet
    {
        glm::vec2 pos;
        glm::vec2 dir;
        float sediment, water, vel;
        int radius;
    };

    class HydraulicErosion : public Program
    {
    public:
        HydraulicErosion();
        ~HydraulicErosion();
        void init() override;
        void clear() override;
        void update(float) override;
        void draw(float) override;

    private:
        glm::vec3 m_light_pos;

        // heightmap
        std::vector<Vertex> m_height_data;
        std::vector<glm::vec4> m_biome_colors;
        ColorGenerator* m_color_gen = nullptr;
        VertexBuffer* m_terrain = nullptr;
        Shader* m_shader = nullptr;
        Shader* m_point_shader = nullptr;

        glm::vec2 m_grid_size;

        std::vector<Vertex> m_droplets;
		VertexBuffer* m_water = nullptr;
        int m_n_drops = 0;                      // Keeps track of how many drops we have already simulated
        DropletSettings m_ds;

        const int m_MAX_DROPS = 100000;
        const int m_DROPS_PER_ITER = 1000;
        const int m_MAX_STEPS = 64;         // Maximum number of steps a droplet will take.

        void distribute(Droplet&, glm::vec2 pos, float sediment);
        void erode(Droplet&, glm::vec2 pos, float amount);

        GLenum m_draw_mode = GL_TRIANGLES;
        bool m_draw_terrain = true;
        void drawTerrain(glm::mat4& pvMatrix, glm::mat4 model);
        bool m_add_droplets = false;
        void drawDroplets(glm::mat4& pvMatrix, glm::mat4 model);
        void simulateDroplet(float x, float y);
        void generateGrid(glm::vec2 size, float frequency = 1.f, float amplitude = 1.f, int octaves = 1);
	    // Bilinearly interpolates point within a quad
        float height_bli(float _x, float _y);
        float height(int x, int y);
        void addHeight(int x, int y, float height);
    };
}