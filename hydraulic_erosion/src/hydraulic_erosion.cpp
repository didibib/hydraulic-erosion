#include "hepch.h"
#include "hydraulic_erosion.h"

namespace he
{
    HydraulicErosion::HydraulicErosion()
    {
    }
    void HydraulicErosion::init()
    {
        generateHeightMapData(glm::vec2(512, 512));
    }

    void HydraulicErosion::update(float)
    {
        std::printf("update\n");
    }

    void HydraulicErosion::draw(float)
    {
        std::printf("draw\n");
    }

    std::vector<float> HydraulicErosion::generateHeightMapData(glm::vec2 size)
    {
        for (int y = 0; y < size.y; y++)
        {
            for (int x = 0; x < size.x; x++)
            {

            }
        }
        return std::vector<float>();
    }
}
