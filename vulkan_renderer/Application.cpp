
#include "PCH.h"

#include <algorithm>
#include <execution>

f32
GetTime(i64 awake)
{
  return (f32)((std::chrono::steady_clock::now().time_since_epoch().count() - awake) / 1000000000.0);
}

int
RunApplication(Application* app)
{

  //create variable to hold how many billboards we have
  std::atomic<int> vertex{0};

  std::vector<int> vec_int = {0,0,0,0};

  std::for_each(std::execution::par, 
            vec_int.begin(),
            vec_int.end(),
            [](int& Part){ 
              Part++;

              printf("vecInt: %d\n", Part);
            });   

  return 0;

  InitWindow(&app->window, 800, 600, "vulkan");
  InitCamera(&app->window.camera);
  Input input;

  app->awakeTime = std::chrono::steady_clock::now().time_since_epoch().count();
  f32 t = 0;
  f32 current_time = GetTime(app->awakeTime);
  f32 accumulator = 0.0f;
  f32 new_time;
  f32 delta_time;

  while (!IsClosed(&app->window))
  {
    new_time = GetTime(app->awakeTime);

    delta_time = new_time - current_time;

    if (delta_time > 0.25f)
    {
      delta_time = 0.25f;
    }

    current_time = new_time;

    accumulator += delta_time;

    PollEvents();

    UpdateInput(&input, &app->window);

    UpdateCamera(&app->window.camera, &input, delta_time);

    while (accumulator >= FIXED_TIME)
    {
      // fixed update

      accumulator -= FIXED_TIME;
      t += FIXED_TIME;
    }

    DrawFrame(&app->window);
  }

  OnWindowClose(&app->window);

  return 0;
}