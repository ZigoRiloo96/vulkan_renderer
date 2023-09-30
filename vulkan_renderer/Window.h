#pragma once

struct
Window
{
	GLFWwindow* glfw_window;
	Renderer renderer;
	Camera camera;
};

void
CreateWindow(Window* window, uint32_t width = 800, uint32_t height = 600, const char* title = "Default");

void
ResizeWindow(Window* window);

static void
framebufferResizeCallback(GLFWwindow* window, int width, int height);

void
InitWindow(Window* window, uint32_t width = 800, uint32_t height = 600, const char* title = "Default");

void
TermWindow(Window* window);

void
PollEvents();

void
DrawFrame(Window* window);

bool
IsClosed(Window* window);

void
OnWindowClose(Window* window);