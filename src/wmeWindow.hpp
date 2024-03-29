#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace wme
{
	class WmeWindow
	{
	public:
		WmeWindow(int width, int height, std::string name);
		~WmeWindow();
		WmeWindow(const WmeWindow&) = delete;
		WmeWindow& operator= (const WmeWindow&) = delete;

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
		bool shouldClose() { return glfwWindowShouldClose(window); }
		VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
		GLFWwindow* getGLFWwindow() const { return window; }
		double getCursorX() { return cursorX; }
		double getCursorY() { return cursorY; }
		bool wasWindowResized() { return framebufferResized; }
		void resetWindowResizedFlag() { framebufferResized = false; }

	private:
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
		static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
		void initWindow();

		int width;
		int height;
		double cursorX;
		double cursorY;
		bool framebufferResized = false;

		std::string windowName;
		GLFWwindow* window;
	};
}