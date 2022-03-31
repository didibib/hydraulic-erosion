#pragma once

namespace he
{
	// Some usefull info about the camera matrices
	// https://gamedev.stackexchange.com/questions/178643/the-view-matrix-finally-explained
	enum class Direction { Forward, Right, Up, Backward, Left, Down };

	class Camera
	{
	public:
		Camera();
		void init(const float& fov, const int& viewWidth, const int& viewHeight, const float& nearPlane, const float& farPlane);

		void move(const Direction& dir, const float& deltaTime);
		void cursor(float xOffset, float yOffset);
		void onWindowResize(const int& width, const int& height);

		const glm::vec3& getPos();
		void setPos(glm::vec3 const& pos);
		void setPos(float const& x, float const& y, float const& z);
		void rotateAroundPoint(glm::vec3 target, float radius, float speed = 1);
		void reset(bool rotate_camera);
		void zoom(float offset);

		const glm::mat4& getProjMatrix();
		// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
		glm::mat4 getViewMatrix();
	private:
		void updateCameraVectors();
		float m_fov;
		float m_near;
		float m_far;
		int m_view_width;
		int m_view_height;

		float m_yaw;
		float m_pitch;

		float m_move_speed = 100;
		float m_mouse_sensivity = 0.8f;
		glm::vec3 m_position;
		glm::vec3 m_forward;
		glm::vec3 m_up;
		glm::vec3 m_right;
		glm::vec3 m_world_up;
		glm::mat4 m_projection;
		glm::mat4 m_view;
	};
}