#pragma once

#include "program.h"

namespace he
{
    class HydraulicErosion : public Program
    {
    public:
        HydraulicErosion();
        void init();
        void update(float) override;
        void draw(float) override;

    private:
        // heightmap

        // Maximum number of steps a droplet will take.
        int m_max_steps;


        // Something something droplet functions
        std::vector<float> generateHeightMapData(glm::vec2 size);
    };
}