#pragma once

//static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
//{
//	auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
//	app->framebufferResized = true;
//}

class glfwWindow
{
private:
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) 
	{
		auto w = reinterpret_cast<glfwWindow*>(glfwGetWindowUserPointer(window));
		w->Resize();
	}

public:
	glfwWindow(uint32_t width = 800, uint32_t height = 600, const char* title = "Default") 
		:vulkan(getRequiredExtensions(), createWindow(width, height, title))
	{
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	~glfwWindow()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	GLFWwindow* createWindow(uint32_t width = 800, uint32_t height = 600, const char* title = "Default")
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, title, nullptr, nullptr);

		glfwSetWindowUserPointer(window, this);

		return window;
	}

	void PollEvents()
	{
		glfwPollEvents();
	}

	void DrawFrame()
	{
		vulkan.DrawFrame();
	}

	bool IsClosed()
	{
		return glfwWindowShouldClose(window);
	}

	void OnWindowClose()
	{
		vulkan.WaitIdle();
	}

	void Resize()
	{
		vulkan.SetFramebufferResize(true);
	}

private:

	std::vector<const char*> getRequiredExtensions()
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

private:
	GLFWwindow* window;
	glfwVulkan vulkan;
};