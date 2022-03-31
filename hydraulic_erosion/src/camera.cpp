#include "hepch.h"
#include "camera.h"

namespace he
{
	Camera::Camera()
		: m_yaw(-90), m_pitch(0), 
		m_forward(0.f, -1.f, 0.f), m_up(0.f, 0.f, 1.f), m_right(1.f, 0.f, 0.f), m_world_up(0.f, 0.f, 1.f)
	{
	}
	void Camera::init(const float& fov, const int& viewWidth, const int& viewHeight, const float& nearPlane, const float& farPlane)
		// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector 
		// pointing to the right so we initially rotate a bit to the left.
	{
		m_fov = fov;
		m_view_width = viewWidth;
		m_view_height = viewHeight;
		m_near = nearPlane;
		m_far = farPlane;

		m_projection = glm::perspective(glm::radians(m_fov), (float)m_view_width / (float)m_view_height, m_near, m_far);
		updateCameraVectors();
	}

	void Camera::move(const Direction& dir, const float& deltaTime)
	{
		float velocity = m_move_speed * deltaTime;
		switch (dir)
		{
		case Direction::Forward:
			m_position += m_forward * velocity;
			break;
		case Direction::Backward:
			m_position -= m_forward * velocity;
			break;
		case Direction::Right:
			m_position += m_right * velocity;
			break;
		case Direction::Left:
			m_position -= m_right * velocity;
			break;
		case Direction::Up:
			m_position += m_up * velocity;
			break;
		case Direction::Down:
			m_position -= m_up * velocity;
			break;
		default:
			break;
		}
		updateCameraVectors();
	}

	void Camera::cursor(float xOffset, float yOffset)
	{
		xOffset *= m_mouse_sensivity;
		yOffset *= m_mouse_sensivity;
		m_yaw = glm::mod(m_yaw + xOffset, 360.f);
		m_pitch += yOffset;
		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		m_pitch = glm::clamp(m_pitch, -89.f, 89.f);
		updateCameraVectors();
	}

	void Camera::updateCameraVectors()
	{
		// Calculate the new mForward vector
		glm::vec3 forward;
		forward.x = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		forward.y = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		forward.z = sin(glm::radians(m_pitch));
		m_forward = glm::normalize(forward);
		// Also re-calculate the Right and Up vector
		// normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		m_right = glm::normalize(glm::cross(m_forward, m_world_up));
		m_up = glm::normalize(glm::cross(m_right, m_forward));
		m_view = glm::lookAt(m_position, m_position + m_forward, m_up);
	}

	void Camera::onWindowResize(const int& width, const int& height)
	{
		if (width > 0 && height > 0)
		{
			m_view_width = width;
			m_view_height = height;
			m_projection = glm::perspective(glm::radians(m_fov), (float)m_view_width / (float)m_view_height, m_near, m_far);
		}
	}

	const glm::vec3& Camera::getPos()
	{
		return m_position;
	}

	void Camera::setPos(glm::vec3 const& pos)
	{
		m_position = pos;
	}

	void Camera::setPos(float const& x, float const& y, float const& z)
	{
		m_position.x = x;
		m_position.y = y;
		m_position.z = z;
		updateCameraVectors();
	}

	void Camera::rotateAroundPoint(glm::vec3 target, float radius, float speed)
	{
		float angle = glfwGetTime() * speed;
		float camX = cos(angle) * radius - sin(angle) * radius;
		float camY = sin(angle) * radius + cos(angle) * radius;
		m_view = glm::lookAt(glm::vec3(camX, camY, m_position.z), target, m_up);
	}

	void Camera::reset(bool rotate_camera)
	{
		m_forward = glm::vec3(0.f, -1.f, 0.f);
		m_up = glm::vec3(0.f, 0.f, 1.f);
		m_right = glm::vec3(1.f, 0.f, 0.f);
		if (!rotate_camera) updateCameraVectors();
	}

	void Camera::zoom(float offset)
	{
		m_fov -= offset;
		if (m_fov < 1.0f)
			m_fov = 1.0f;
		if (m_fov > 60.f)
			m_fov = 60.0f;
	}

	const glm::mat4& Camera::getProjMatrix()
	{
		return m_projection = glm::perspective(glm::radians(m_fov), (float)m_view_width / (float)m_view_height, m_near, m_far);
	}

	glm::mat4 Camera::getViewMatrix()
	{
		return m_view;
	}
}