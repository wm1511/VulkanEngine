#include "wmeWindow.hpp"

#include <stdexcept>

namespace wme
{
	WmeWindow::WmeWindow(int width, int height, std::string name) : width(width), height(height), windowName(name)
	{
		initWindow();
	}

	WmeWindow::~WmeWindow()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void WmeWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
			throw std::runtime_error("failed to create window surface");
	}

	void WmeWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto wmeWindow = reinterpret_cast<WmeWindow*>(glfwGetWindowUserPointer(window));
		wmeWindow->framebufferResized = true;
		wmeWindow->width = width;
		wmeWindow->height = height;
	}

	void WmeWindow::initWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetWindowSizeCallback(window, framebufferResizeCallback);
	}
}
