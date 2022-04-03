#include "hepch.h"
#include "util.h"
#include "vertex_buffer.h"

namespace he
{
	VertexBuffer::VertexBuffer(std::string name) :
		m_is_bind(false), m_n_vertices(0), m_n_indices(0),
		m_vao(0), m_vbo(0), m_ebo(0),
		m_name(name)
	{
	}

	void VertexBuffer::init(const std::vector<Vertex>& vertices)
	{
		init(vertices, std::vector<util::uint>());
	}

	void VertexBuffer::init(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices)
	{
		m_n_vertices = vertices.size();
		m_n_indices = indices.size();

		TRACE("Creating buffers: [vertices:{}][indices:{}]", m_n_vertices, m_n_indices);

		// Generate buffers
		glGenVertexArrays(1, &m_vao);
		glGenBuffers(1, &m_vbo);
		// Bind VAO
		glBindVertexArray(m_vao);
		// Bind VBO
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_n_vertices, &vertices[0], GL_STATIC_DRAW);

		if (!indices.empty())
		{
			// Bind EBO
			TRACE("Generating EBO");
			glGenBuffers(1, &m_ebo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_n_indices, &indices[0], GL_STATIC_DRAW);
		}
		// Set attributes
		// Position (location = 0)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		glEnableVertexAttribArray(0);
		// Color (location = 1)
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
		glEnableVertexAttribArray(1);
		// UV (location = 2) 
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
		glEnableVertexAttribArray(2);
		// Normal (location = 3)
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		glEnableVertexAttribArray(3);

		// Unbind buffers
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored
		glBindVertexArray(0);
	}

	//https://stackoverflow.com/questions/34125298/modify-single-vertex-position-in-opengl
	void VertexBuffer::update(std::vector<Vertex> const& vertices)
	{
		m_n_vertices = vertices.size();
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_n_vertices, &vertices[0], GL_DYNAMIC_DRAW);
		glBindVertexArray(0);
	}

	void VertexBuffer::bind() const
	{
		glBindVertexArray(m_vao);
		m_is_bind = true;
	}

	void VertexBuffer::unbind() const
	{
		glBindVertexArray(0);
		m_is_bind = false;
	}

	void VertexBuffer::draw(GLenum mode) const
	{
		if (!m_is_bind)
		{
			ERROR("Forgot to bind the buffer for: {}", m_name);
			return;
		}
		if (m_ebo != 0)
		{
			glDrawElements(mode, m_n_indices, GL_UNSIGNED_INT, NULL);
		}
		else
		{
			glDrawArrays(mode, 0, m_n_vertices);
		}
	}
}