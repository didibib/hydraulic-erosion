#pragma once

#include "program.h"
#include "vertex.h"

namespace he
{
    class VertexBuffer;
    class Shader;
    class ColorGenerator;

    struct GlobalParameters
    {
        float rain_rate = .05;                // K_r      : [0.0;0.05]
        float water_evap_rate = 0.035;      // K_e      : [0.0;0.05]
        float gravity = 9.81;               // g        : [0.1;20]
        float sediment_capacity = 3;        // K_c      : [0.1;3]
        float thermal_erosion_rate = .5;  // K_t      : [0.0;3]  
        float cross_section_area = 20;      // A        : [0.1;60]
        float min_local_tilt_angle = 0.1f;
        float soil_suspension_rate = 0.5f;  // K_s      : [0.1;2]
        float sediment_depo_rate = 1.f;     // K_d      : [0.1;3]
        float sediment_soft_rate = 5;       // K_h      : [0.0;10]
        float max_erosion_depth = 10;       // K_dmax   : [0.0;40] 
        float talus_angle_tang_coeff = 0.8; // K_a      : [0.0;1]
        float talus_angle_tang_bias = 0.1;  // K_i      : [0.0;1]
        float talus_angle = 0.5;
        float hardness_lower_limit = 0.3;   // R_min    : [0.0;1]
        float water_z_offset = 1.f;         // delta y for rendering
    };

    struct GridPoint
    {
        GridPoint(float _b, float _r, float _R){
            b = _b;
            tempB = 0;
            d = 0;
            s = 0;
            tempS = 0;
            flux = std::vector<float>(4, 0);
            pipes = std::vector<float>(8, 0);
            v = glm::vec2(0);
            r = _r;
            R = _R;
        }
        // Terrain height
        float b;
        float tempB;
        // Water height
        float d;
        // Suspended sediment amount
        float s;
        float tempS;
        // Water outflow flux
        std::vector<float> flux;
        // Soil outflow pipes
        std::vector<float> pipes;
        // Velocity vector
        glm::vec2 v;
        // Constant rain rate
        float r;
        // Local hardness coefficient
        float R;
    };

    class ShallowErosion : public Program
    {
    public:
        ShallowErosion();
        ~ShallowErosion();
        void init() override;
        void clear() override;
        void update(float) override;
        void draw(float) override;

    private:
        // Data
        glm::vec2 m_grid_size;
        std::vector<Vertex> m_height_data;
        std::vector<Vertex> m_water_data;
        // Contains the information necessary for shallow water model
        std::vector<GridPoint> m_grid_data;
        GlobalParameters m_gp;
        std::string m_texture_key;

        int m_iterations;

        // Rendering
        glm::vec3 m_light_pos;
        std::vector<glm::vec4> m_biome_colors;
        ColorGenerator* m_color_gen = nullptr;
        VertexBuffer* m_terrain = nullptr;
        VertexBuffer* m_water = nullptr;
        Shader* m_shader = nullptr;
        Shader* m_water_shader = nullptr;
        Shader* m_point_shader = nullptr;
        Shader* m_matcap_shader = nullptr;
        GLenum m_draw_mode = GL_TRIANGLES;
        bool m_draw_terrain = true;
        bool m_draw_water = false;
        bool m_iterate = false;

        bool m_i_pressed = false;

        const std::vector<glm::ivec2> m_directions = {
            // First 4 are horizontal and vertical 
            {-1, 0}, {0, -1}, {1, 0}, {0, 1}, 
            // Last 4 are diagonal 
            {-1, -1}, {1, -1}, {1, 1}, {-1, 1}
        };

        void drawTerrain(glm::mat4& pvMatrix, glm::mat4 model);
        void drawWater(glm::mat4& pvMatrix, glm::mat4 model);
        void generateGrid(glm::vec2 size, float frequency = 1.f, float amplitude = 1.f, int octaves = 1);
        void generateSphere(glm::ivec2 size, float r);
        void createTerrainMesh(glm::vec2 size);

        void incrementWater(float deltaTime);
        void updateFlow(float deltaTime);
        void erodeAndDeposit(float deltaTime);
        void transportSediment(float deltaTime);
        float sediment(int x, int y);
        void evaporateWater(float deltaTime);
        void updateVertexHeight();

        void calculateSoilFlow(float deltaTime);

        void applyThermalErosion();
        
        float localTiltAngle(int index);
        float shallowDeepLimiter(float d);
        std::optional<glm::ivec2> getNewPos(int index, glm::ivec2 dir);
    };
}