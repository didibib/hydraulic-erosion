#include "hepch.h"
#include "window.h"
#include "util.h"
#include "program.h"

namespace he
{
	Window& WINDOW = Window::getInstance();

	Window::~Window()
	{
		glfwDestroyWindow(m_glfw_window);
		glfwTerminate();
	}

	/**
	 * OpenGL context initialisation
	 */
	bool Window::init(int width, int height, const char* win_name)
	{
		glfwInit();
		// Window hints need to be set before the creation of the window. They function as additional arguments to glfwCreateWindow.
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		m_glfw_window = glfwCreateWindow(width, height, win_name, nullptr, nullptr);

		if (!m_glfw_window)
		{
			CRITICAL("Failed to create window");
			return false;
		}
		glfwSetWindowUserPointer(m_glfw_window, this);
		glfwMakeContextCurrent(m_glfw_window);

		// Load OpenGL extensions
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			CRITICAL("Failed to initialize OpengGL context");
			return false;
		}

		glEnable(GL_DEPTH_TEST); 
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Setup callbacks
		glfwSetKeyCallback(m_glfw_window, keyCallback);
		glfwSetMouseButtonCallback(m_glfw_window, mouseCallback);
		glfwSetScrollCallback(m_glfw_window, scrollCallback);
		glfwSetCursorPosCallback(m_glfw_window, cursorCallback);
		glfwSetFramebufferSizeCallback(m_glfw_window, frameBufferCallback);
		glfwSetCursorEnterCallback(m_glfw_window, cursorEnterCallback);
		glfwSetWindowFocusCallback(m_glfw_window, windowFocusCallback);

		m_camera = new Camera(90, width, height, 0.001, 1000);
		m_camera->setPos({ 200, 200, 10 });

		return true;
	}

	void Window::run(Program* program)
	{
		program->init();
		static float currentTime = 0, previousTime = 0;
		while (!glfwWindowShouldClose(m_glfw_window))
		{
			currentTime = glfwGetTime();
			m_delta_time = currentTime - previousTime;
			previousTime = currentTime;

			handleCameraInput();

			program->clear();
			program->update(m_delta_time);
			program->draw(m_delta_time);

			glfwSwapBuffers(m_glfw_window);
			glfwPollEvents();
		}
	}

	void Window::handleCameraInput()
	{
		if (!m_rotate_camera)
		{
			if (isKeyRepeat(GLFW_KEY_W)) m_camera->move(Direction::Forward, m_delta_time);
			if (isKeyRepeat(GLFW_KEY_A)) m_camera->move(Direction::Left, m_delta_time);
			if (isKeyRepeat(GLFW_KEY_S)) m_camera->move(Direction::Backward, m_delta_time);
			if (isKeyRepeat(GLFW_KEY_D)) m_camera->move(Direction::Right, m_delta_time);
		}
		if (isKeyRepeat(GLFW_KEY_LEFT_CONTROL))  m_camera->move(Direction::Down, m_delta_time);
		if (isKeyRepeat(GLFW_KEY_SPACE))  m_camera->move(Direction::Up, m_delta_time);
	}

	bool Window::isKeyRepeat(int key)
	{
		return WINDOW.m_key_lookup[key] == GLFW_PRESS || WINDOW.m_key_lookup[key] == GLFW_REPEAT;
	}

	bool Window::isKeyPressed(int key)
	{
		return WINDOW.m_key_lookup[key] == GLFW_PRESS;
	}

	bool Window::isKeyReleased(int key)
	{
		return WINDOW.m_key_lookup[key] == GLFW_RELEASE;
	}

	Camera& Window::getCamera()
	{
		return *WINDOW.m_camera;
	}

#pragma region __CALLBACKS
	void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		WINDOW.m_key_lookup[key] = action;
	}

	void Window::mouseCallback(GLFWwindow* window, int button, int action, int mods)
	{

	}

	void Window::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		WINDOW.m_camera->zoom(yoffset);
	}

	void Window::cursorCallback(GLFWwindow* window, double xpos, double ypos)
	{
		if (WINDOW.m_reset_cursor)
		{
			WINDOW.m_cursor_last_pos = glm::vec2(xpos, ypos);
			WINDOW.m_reset_cursor = false;
		}
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS
			&& !WINDOW.m_rotate_camera)
		{
			WINDOW.m_camera->cursor(xpos - WINDOW.m_cursor_last_pos.x, WINDOW.m_cursor_last_pos.y - ypos);
		}
		WINDOW.m_cursor_last_pos = glm::vec2(xpos, ypos);
	}

	void Window::frameBufferCallback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
		WINDOW.m_camera->onWindowResize(width, height);
	}

	void Window::cursorEnterCallback(GLFWwindow* window, int entered)
	{
		WINDOW.m_reset_cursor = entered;
	}

	void Window::windowFocusCallback(GLFWwindow* window, int focused)
	{
		WINDOW.m_reset_cursor = focused;
	}

#pragma endregion
	
} /* namespace he */
