#pragma once

namespace he
{
	class Shader;
	class Camera;
	class Program;

	// https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
	class Window
	{
		Window() {};

		float m_delta_time = 0;
		bool m_paused = true;
		bool m_draw_voxels = true;

		glm::vec4 m_clear_color;

		GLFWwindow* m_glfw_window = nullptr;
		Camera* m_camera = nullptr;
		bool m_rotate_camera = false;
		bool m_reset_cursor = false;
		glm::vec2 m_cursor_last_pos;


		std::map<int, int> m_key_lookup;
		bool isKeyRepeat(int);
		bool isKeyPressed(int);

	public:
		static Window& getInstance()
		{
			static Window instance;
			return instance;

		}
		~Window();
		Window(Window const&) = delete;
		void operator=(Window const&) = delete;

		bool init(int, int, const char*);
		void clear();
		void run(Program*);

		static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void mouseCallback(GLFWwindow* window, int button, int action, int mods);
		static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
		static void cursorCallback(GLFWwindow* window, double xpos, double ypos);
		static void frameBufferCallback(GLFWwindow* window, int width, int height);
		static void cursorEnterCallback(GLFWwindow* window, int entered);
		static void windowFocusCallback(GLFWwindow* window, int focused);
	};
} /* namespace he */