
#include "PCH.h"

std::vector<const char*> GetRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

void
CreateWindow(Window* window, uint32_t width, uint32_t height, const char* title)
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	window->glfw_window = glfwCreateWindow(width, height, title, nullptr, nullptr);

	glfwSetWindowUserPointer(window->glfw_window, window);
}

void
ResizeWindow(Window* window)
{
	window->renderer.framebufferResized = true;
}

static void
framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto w = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	ResizeWindow(w);
}

void
InitWindow(Window* window, uint32_t width, uint32_t height, const char* title)
{
	CreateWindow(window, width, height, title);

	InitRenderer(window, GetRequiredExtensions());

	glfwSetFramebufferSizeCallback(window->glfw_window, framebufferResizeCallback);
}

void
TermWindow(Window* window)
{
	TermRenderer(&window->renderer);

	glfwDestroyWindow(window->glfw_window);
	glfwTerminate();
}

void
PollEvents()
{
	glfwPollEvents();
}

void
DrawFrame(Window* window)
{
	DrawFrame(&window->renderer);
}

bool
IsClosed(Window* window)
{
	return glfwWindowShouldClose(window->glfw_window);
}

void
OnWindowClose(Window* window)
{
	WaitIdle(&window->renderer);
}