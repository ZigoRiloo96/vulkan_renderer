#pragma once



class Application
{
public:

  Application() : window(800, 600)
  {
    // init Vulkan
  }

  ~Application()
  {
    // cleanup
  }

  int run()
  {
    // loop

    while (!window.IsClosed())
    {
      window.PollEvents();
      window.DrawFrame();
    }

    window.OnWindowClose();

    return 0;
  }

private:

  glfwWindow window;

};