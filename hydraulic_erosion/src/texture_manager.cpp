#include "hepch.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "texture_manager.h"

namespace he
{
	std::optional<Texture> TextureManager::Load(const std::string& filepath, const std::string& userKey)
	{
		// Check if the texture is already loaded
		auto it = m_texture_map.find(filepath);
		if (it != m_texture_map.end())
		{
			return (*it).second;			
		}

		it = m_texture_map.find(userKey);
		if (it != m_texture_map.end())
		{
			return (*it).second;
		}
		TRACE("Loading image: {}", filepath);

		int width, height;
		unsigned char* data = stbi_load(filepath.c_str(), &width, &height, 0, 0);

		if (data == nullptr)
		{
			ERROR("Failed to load image: {}", filepath);
			return std::nullopt;
		}

		TRACE("{} : [width:{}][height:{}]", filepath, width, height);

		GLuint id;
		glGenTextures(1, &id);
		if (id == 0)
		{
			ERROR("Failed to generate texture id");
			return std::nullopt;
		}
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);

		std::string key = filepath;
		if (userKey != "")
		{
			key = userKey;
		}		

		// Add texture
		Texture texture(id, width, height);
		AddTexture(key, texture);

		return texture;
	}

	void TextureManager::Bind(const std::string& key, GLenum textureUnit)
	{
		auto it = m_texture_map.find(key);
		if (it != m_texture_map.end())
		{
			auto texture = (*it).second;
			glActiveTexture(textureUnit);
			glBindTexture(GL_TEXTURE_2D, texture.Id);
		}
	}

	void TextureManager::Unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void TextureManager::AddTexture(std::string key, Texture texture)
	{
		auto pair = std::pair<std::string, Texture>(key, texture);
		m_texture_map.insert(pair);
		TRACE("Added texture: {}", key);
	}

	void TextureManager::DeleteTexture(const std::string& key)
	{
		auto it = m_texture_map.find(key);
		if (it != m_texture_map.end())
		{
			m_texture_map.erase(it);
			TRACE("Deleted texture: {}", key);
		}
	}
}