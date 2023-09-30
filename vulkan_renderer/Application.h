#pragma once

struct 
Application
{
  Window window;
  i64 awakeTime;
};

int
RunApplication(Application* app);