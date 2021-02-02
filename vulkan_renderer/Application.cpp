
#include "PCH.h"

int
RunApplication(Application* app)
{
  // loop

  InitWindow(&app->window, 800, 600, "def");

  while (!IsClosed(&app->window))
  {
    PollEvents();
    DrawFrame(&app->window);
  }

  OnWindowClose(&app->window);

  return 0;
}