namespace he
{
    class HeightMap
    {
        public:
            HeightMap(glm::vec2 size, float[] data) : m_size(size), m_data(data) {}

            double at(int x, int y) 
            { 
                return m_data[y * m_size + x] 
            }


        private:
            glm::vec2 m_size;
            float[] m_data;
    }
}