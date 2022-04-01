#pragma once

#include "vertex.h"

namespace he
{
	class VertexBuffer
	{
	public:
		VertexBuffer(std::string name);
		void init(std::vector<Vertex> const& vertices);
		void init(std::vector<Vertex> const& vertices, std::vector<unsigned int> const& indices);
		void update(std::vector<Vertex> const& vertices);
		void bind() const;
		void draw(GLenum mode = GL_TRIANGLES) const;
		void unbind() const;
	private:
		mutable bool m_is_bind;
		std::string m_name;
		int m_n_vertices;
		int m_n_indices;
		GLuint m_vao;
		GLuint m_vbo;
		GLuint m_ebo;
	};
}