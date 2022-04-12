#pragma once 

namespace he
{
	struct Texture
	{
		Texture(GLuint _id, int _width, int _height)
		{
			Id = _id;
			Width = _width;
			Height = _height;
		}

		GLuint Id;
		int Width;
		int Height;
	};

	// https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
	class TextureManager
	{
	public:
		static TextureManager& getInstance()
		{
			static TextureManager instance;
			return instance;
		}
		TextureManager(TextureManager const&) = delete;
		void operator=(TextureManager const&) = delete;
		std::optional<Texture> Load(const std::string& filepath, const std::string& userKey = "");
		void Bind(const std::string& key, GLenum textureUnit);
		void Unbind();

	private:
		TextureManager(){}
		std::unordered_map<std::string, Texture> m_texture_map;
		void AddTexture(std::string key, Texture);
		void DeleteTexture(const std::string&);
	};
}