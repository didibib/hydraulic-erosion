namespace he
{
    struct DropletSettings
    {
        float inertia;   // [0,1], how much old_dir a droplet keeps after an update
        float p_capacity; // [0 ..], the total capacity of a droplet
        float p_deposit; // [0,1], the deposition speed of the sediment in a droplet
        float p_erosion;   // [0,1], the erosion speed of a droplet
        float p_evaporation;  // [0,1], the evaporation speed of a droplet
        int ero_r;       // [0 ..], the erosion radius of a droplet
        float min_slope;
        float gravity;
    };

    class Droplet
    {
    public:
        Droplet(DropletSettings const& ds) : m_settings(ds) {}

    private:
        DropletSettings const& m_settings;
        int m_cur_steps;
        glm::vec2 m_pos;
        glm::vec2 m_dir;
        float m_vel, m_water, m_sediment;
    }
}